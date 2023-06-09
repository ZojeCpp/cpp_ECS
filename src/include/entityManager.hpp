
#pragma once
#include <cstddef>
#include <vector>
#include <cstdint>
#include <cassert>
#include <include/utilities/componentStorage.hpp>



namespace zoje {
    //Entity Manager class
    //Templates allow to use different Objects for the same functions
    //example:
    //  EntityManager<Casas>    EMC;
    //  EntityManager<Aviones>  EMA;
    //Functions will work the same way but using different Objects
    template <typename CMP_LIST,typename TAG_LIST,typename UNIQUE_CMP_LIST,size_t Capacity = 10>
    struct EntityManager{
        
        using cmp_storer_t = cmp::storer<CMP_LIST,TAG_LIST,UNIQUE_CMP_LIST,Capacity>;
        template<typename T>
        using to_key_type    = typename zoje::static_slotmap<T,Capacity>::key_type;

        struct Entity{
			// template<typename CMP_LIST,typename TAG_LIST, size_t Capacity = 10>
			// friend struct EntityManager;

            using keytype_list  = ppUtils::foreach_element_insert_t<to_key_type,CMP_LIST>;
            using key_storage_t = ppUtils::replacer_t<std::tuple, keytype_list>;

            /// @brief adds the component bit_mask to the component mask of the entity and the key to the component
            /// @tparam CMP 
            /// @param key depending on byte size it will be passed as reference or as lvalue, should be lvalue
            template <typename CMP>
            void addComponent(ppUtils::value_or_reference_t<to_key_type<CMP>> key)
            {
               cmp_mask |= cmp_storer_t::cmp_info::template mask<CMP>();
               std::get< to_key_type<CMP> >(cmp_keys) = key;
            }

            /// @brief check if the entity has the given component
            /// @tparam CMP 
            /// @return true if it has the component, false otherwise
            template <typename CMP>
            [[__nodiscard__]] constexpr const bool hasComponent() const noexcept
            { return (cmp_mask & cmp_storer_t::cmp_info::template mask<CMP>()); }

            /// @brief checks if the entity has all the components listed
            /// @tparam ...CMPs 
            /// @return returns true only if the entity has all the components listed, false otherwise
            template <typename... CMPs>
            [[__nodiscard__]] constexpr const bool hasComponents() const noexcept
            { return ( true && ... && hasComponent<CMPs>()); }

            /// @brief returns the key to the entity component given as template parameter
            /// @tparam CMP 
            /// @return a const reference or a const lvalue of the key_type, which one depends on byte size of key_type, should be const lvalue 
            template <typename CMP>
            [[__nodiscard__]] constexpr const ppUtils::value_or_reference_t<to_key_type<CMP>> getComponentKey() const
            {
                assert(hasComponent<CMP>() && "Error: The entity doesn't have the requested Component"); 
                return std::get<to_key_type<CMP>>(cmp_keys);
            }

            /// @brief adds a tag to the entity
            /// @tparam TAG 
            template <typename TAG>
            constexpr void addTag()
            { tag_mask |= cmp_storer_t::tag_info::template mask<TAG>();}

            /// @brief adds a parameter pack of tags to the entity
            /// @tparam ...TAGs 
            template <typename... TAGs>
            constexpr void addTags()
            { (addTag<TAGs>(),...); }
            
            /// @brief checks if the entity has the given flag
            /// @tparam TAG 
            /// @return true if it has the tag, false otherwise
            template <typename TAG>
            [[__nodiscard__]] constexpr const bool hasTag() const noexcept
            { return (tag_mask & cmp_storer_t::tag_info::template mask<TAG>()); }

            /// @brief checks if the entity has all the tags listed
            /// @tparam ...TAGs 
            /// @return returns true only if the entity has all the tags listed, false otherwise
            template <typename... TAGs>
            [[__nodiscard__]] constexpr const bool hasTags() const noexcept
            { return ( true && ... &&  hasTag<TAGs>()); }


            /// @brief check if the entity is alive 
            /// @return true if it's alive, false otherwise
            [[__nodiscard__]] const bool isAlive() const noexcept
            { return alive; }

			constexpr void mark4destruction() noexcept
			{ alive = false;}

         private:

            key_storage_t cmp_keys{};
            std::size_t id{nextID++};
            typename cmp_storer_t::cmp_info::mask_type_t cmp_mask{};
            typename cmp_storer_t::tag_info::mask_type_t tag_mask{};
            bool alive{true};

            inline static std::size_t nextID{1};
        };       
        
        /// @brief 
        /// @param defaultsize 
        EntityManager(std::size_t defaultsize = 100){
            //this is done so that the vector doesnt have to increase its size until we have 101 entities
            //this is  beneficial because every time it resizes, there's a high chance it changes memory location
            entities_.reserve(defaultsize); //set capacity to 100
        }

        /// @brief erases all dead entities and adds newly created entities to the living entities, references to entities obtained through "createEtntity" are invalidated
        constexpr void update() noexcept
        {
        	auto beginDeadEntities = std::stable_partition(entities_.begin(), entities_.end(), [](Entity& e){ if(e.isAlive()) return true; else return false;});

			for(auto it = beginDeadEntities; it!=entities_.end();it++)
			{
				deleteEntityComponents(*it,CMP_LIST{});
			}
			entities_.erase(beginDeadEntities,entities_.end());

            for(auto new_e : new_entities_)
            {
                entities_.emplace_back(new_e);
            }
        }

        /// @brief create a new empty entity  
        /// @return a reference to the created entity, this reference shouldn't be saved as it can be invalidated at any point
        [[__nodiscard__]]auto& createEntity() noexcept
		{ return new_entities_.emplace_back(); }
        
        /// @brief call the given function for each entity currently alive
        /// @param process it can bind to both lvalue and rvalue
        void forall(auto&& process) noexcept
		{
            for(auto& e : entities_)
            {
                if(e.isAlive()) process(e);
            }
        }

        template <typename CMP_PACKAGE, typename TAG_PACKAGE>
        void foreach(auto&& process) noexcept 
        {
            foreach_matching_entity(process, CMP_PACKAGE{},TAG_PACKAGE{});
        }

        template <typename CMP>
        void foreach_cmp(auto&& process) noexcept 
		{
            for(auto cmp : componentStorer_.template getDrawerOf<CMP>())
            {
                process(cmp);
            }
        }

        /// @brief returns the unique component of the type given as template parameter, if the component hasn't been initialized it will have default values
        /// @tparam UNIQ_CMP 
        /// @param e 
        /// @return returns reference to the requested component
        template <typename UNIQ_CMP>
        UNIQ_CMP& getUniqueComponent()
        { return componentStorer_.template getUnique<UNIQ_CMP>(); }

        /// @brief initialize unique component
        /// @tparam UNIQ_CMP 
        /// @tparam ...UNIQ_CMP_ARGS no need to explicitly declare
        /// @param e 
        /// @param ...cmp_args  parameters needed to initialize the component 
        template <typename UNIQ_CMP, typename... UNIQ_CMP_ARGS>
        constexpr void initUniqueComponent(UNIQ_CMP_ARGS&&... cmp_args) noexcept
        { componentStorer_.template insertUnique<UNIQ_CMP>(std::forward<UNIQ_CMP_ARGS>(cmp_args)...); }

		template <typename CMP>
		constexpr void deleteEntityComponent(Entity& e) noexcept
		{
			if(e.template hasComponent<CMP>())
			{
				auto key = e.template getComponentKey<CMP>();
				componentStorer_.template pop<CMP>(key);
			}
		}



        /// @brief returns the component of the type given as template parameter, if the entity doesn't have the component it will throw an assert
        /// @tparam CMP 
        /// @param e 
        /// @return returns reference to the requested component
        template <typename CMP>
        CMP& getComponent(Entity& e)
        {
            assert(e. template hasComponent<CMP>() && "Error: The entity has no component of this kind");

            return componentStorer_.template getDataByKey<CMP>(e.template getComponentKey<CMP>());
        }

        // /// @brief returns the component of the type given as template parameter, if the entity doesn't have the component it will throw an assert
        // /// @tparam CMP 
        // /// @param e 
        // /// @return returns read-only reference to the requested component
        // template <typename CMP>
        // const CMP& cgetComponent(Entity& e) const
        // {
        //     assert(e. template hasComponent<CMP>() && "Error: The entity has no component of this kind");

        //     return const_cast(getComponent<CMP>(e));
        // }

        /// @brief adds component to the given entity
        /// @tparam CMP 
        /// @tparam ...CMP_ARGS no need to explicitly declare
        /// @param e 
        /// @param ...cmp_args  parameters needed to build the component 
        template <typename CMP, typename... CMP_ARGS>
        constexpr void addComponent(Entity& e,CMP_ARGS&&... cmp_args) noexcept
        {
            assert(!e. template hasComponent<CMP>() && "Error: The entity already has a component of this kind");

            auto key = componentStorer_.template insert<CMP>(std::forward<CMP_ARGS>(cmp_args)...);
            e.template addComponent<CMP>(key);
        }


    private:

		template < typename ...CMPs>
		constexpr void deleteEntityComponents(Entity& e, zoje::Package<CMPs...>)
		{
			(deleteEntityComponent<CMPs>(e),...);			
		}

        template <typename... CMPs, typename... TAGs>
        void foreach_matching_entity(auto&& process, zoje::Package<CMPs...>, zoje::Package<TAGs...>) noexcept {

            for(auto& e : entities_)
            {
				auto hasCMPs = e.template hasComponents<CMPs...>(); 
				auto hasTAGs = e.template hasTags<TAGs...>(); 

                if(e.isAlive() && hasCMPs  && hasTAGs) 
                    process(e, getComponent<CMPs>(e)...);
            }
        }


        std::vector<Entity> entities_{};
        std::vector<Entity> new_entities_{};

        cmp_storer_t componentStorer_{};


        

    };
}//namespace PurpleSpriteEngine