
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
    template <typename CMP_LIST, size_t Capacity = 10>
    struct EntityManager{
        
        using cmp_storer_t = cmp::storer<CMP_LIST,Capacity>;
        template<typename T>
        using to_key_type    = typename zoje::static_slotmap<T,Capacity>::key_type;

        struct Entity{


            using keytype_list  = ppUtils::foreach_element_insert_t<to_key_type,CMP_LIST>;
            using key_storage_t = ppUtils::replacer_t<std::tuple, keytype_list>;

            template <typename CMP>
            void addComponent(ppUtils::value_or_reference_t<to_key_type<CMP>> key){
               cmp_mask |= cmp_storer_t::cmp_info::template mask<CMP>();
               std::get< to_key_type<CMP> >(cmp_keys) = key;
            }

            template <typename CMP>
            bool hasComponent() const noexcept
            { return (cmp_mask & cmp_storer_t::cmp_info::template mask<CMP>()); }

            template <typename CMP>
            const ppUtils::value_or_reference_t<to_key_type<CMP>> getComponentKey() const
            {
                assert(!hasComponent<CMP>() && "Error: The entity doesn't have the requested Component"); 
                return std::get<to_key_type<CMP>>(cmp_keys);
            }

            [[__nodiscard__]] bool isAlive() { return alive; }

         private:
            key_storage_t cmp_keys{};
            std::size_t id{nextID++};
            typename cmp_storer_t::cmp_info::mask_type_t cmp_mask{};
            typename cmp_storer_t::cmp_info::mask_type_t tag_mask{};
            bool alive{true};

            inline static std::size_t nextID{1};
        };       

        using func_ptr = void (*)(Entity&);  //macro for function pointer type

        
        /// @brief 
        /// @param defaultsize 
        EntityManager(std::size_t defaultsize = 100){
            //this is done so that the vector doesnt have to increase its size until we have 101 entities
            //this is  beneficial because every time it resizes, there's a high chance it changes memory location
            entities_.reserve(defaultsize); //set capacity to 100
        }

        // void update()
        // {
        //     for(auto e : new_entities_)
        //     {
        //         entities_.push_back(e);
        //     }
        // }

        template <typename CMP, typename... CMP_ARGS>
        void addComponent(Entity& e,CMP_ARGS&&... cmp_args)
        {
            assert(!e. template hasComponent<CMP>() && "Error: The entity already has a component of this kind");

            auto key = componentStorer_.template insert<CMP>(std::forward<CMP_ARGS>(cmp_args)...);
            e.template addComponent<CMP>(key);

        }


        /// @brief create a new empty entity  
        /// @return a reference to the created entity, this reference shouldn't be saved as it can be invalidated at any point
        [[__nodiscard__]]auto& createEntity(){
            return entities_.emplace_back();
        }
        
        /// @brief call the given function for each entity currently alive
        /// @param process 
        void forall(func_ptr process){
            for(auto& e : entities_)
            {
                if(e.isAlive()) process(e);
            }
        }

    private:
        std::vector<Entity> entities_{};
        //std::vector<Entity> new_entities_{};

        cmp_storer_t componentStorer_{};


        

    };
}//namespace PurpleSpriteEngine