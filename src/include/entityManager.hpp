
#pragma once
#include <cstddef>
#include <vector>
#include <cstdint>
#include <include/utilities/componentStorage.hpp>



namespace zoje {
    //Entity Manager class
    //Templates allow to use different Objects for the same functions
    //example:
    //  EntityManager<Casas>    EMC;
    //  EntityManager<Aviones>  EMA;
    //Functions will work the same way but using different Objects
    template <typename CMP_LIST>
    struct EntityManager{
        
        struct Entity{
            constexpr static std::size_t Capacity = 10;
            using cmp_storer_t = cmp::storer<CMP_LIST,Capcity>;

            template<typename T>
            using to_key_type    = typename Slotmap<T,Capacity>::key_type;
            using keytype_list  = MP::forall_insert_template_t<to_key_type,CMP_LIST>;
            using key_storage_t = MP::replace_t<std::tuple, keytype_list>;

            //static_assert(MP::is_same_v<key_storage_t,void>);

            template <typename CMP>
            //habria q hacer esto por metaprogramacion pa q si es grande lo pase por referencia
            void addComponent(to_key_type<CMP> key){
               cmp_mask |= cmp_storer_t::cmpinfo::template mask<CMP>();
               std::get< to_key_type<CMP> >(cmp_keys) = key;
            }

         private:
            key_storage_t cmp_keys{};
            std::size_t id{nextID++};
            typename cmp_storer_t::cmpinfo::mask_type_t cmp_mask{};
            typename cmp_storer_t::cmpinfo::mask_type_t tag_mask{};

            inline static std::size_t nextID{1};
        };       
        
        using func_ptr = void (*)(Entity&);  //macro for function pointer type
        
        EntityManager(std::size_t defaultsize = 100){
            //this is done so that the vector doesnt have to increase its size until we have 101 entities
            //this is  beneficial because every time it resizes, there's a high chance it changes memory location
            entities_.reserve(defaultsize); //set capacity to 100
        }
        //we use emplace to directly create the Entity inside the vector
        //If we used push_back we would have to first create the object and then copy it on the vector
        //auto& createEntity(){ return entities_.emplace_back(); }
        auto& createEntity(){
            return entities_.emplace_back();
        }
        //We define a ForAll function with a function pointer
        void forall(func_ptr process){
            for(auto& e : entities_)
                process(e);
        }

    private:
        std::vector<EntityType> entities_{};
        cmp_storer_t components_{};


        

    };
}//namespace PurpleSpriteEngine