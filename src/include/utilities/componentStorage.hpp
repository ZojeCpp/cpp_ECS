#pragma once
#include <include/utilities/slotmap.hpp>
#include <include/utilities/type_pack.hpp>
#include <cstdint>


namespace zoje 
{
    namespace cmp
    {
        template<typename CMP_LIST, size_t Capacity = 10>
        struct storer
        {
            using cmp_info = CMP_LIST;
            template<typename T>
            using to_key_type    = typename zoje::static_slotmap<T,Capacity>::key_type;
            //using tag_info = TAG_LIST;


            // template <typename CMP>
            // [[__nodiscard__]] static constexpr size_t getId() noexcept
            // {
            //     static_assert(CMP_LIST:: template contains<CMP>());
            //     return CMP_LIST:: template position_of<CMP>();
            // }

            // template <typename CMP>
            // [[__nodiscard__]] static constexpr CMP_LIST::mask_type getMask() noexcept
            // {
            //     static_assert(CMP_LIST:: template contains<CMP>());
            //     return CMP_LIST::mask();
            // }


            template<typename CMP>
            [[__nodiscard__]] constexpr auto& getDrawerOf() noexcept
            {
                static_assert(CMP_LIST::  template contains<CMP>());
                return std::get<CMP_LIST:: template position_of<CMP>()>(storage_);
            }

            template<typename CMP>
            [[__nodiscard__]] constexpr auto& getDataByKey(to_key_type<CMP> key) noexcept
            {
                auto& drawer = getDrawerOf<CMP>();
                return drawer[key];
            }


            template<typename CMP>
            [[__nodiscard__]] constexpr auto insert(ppUtils::r_reference_or_l_reference_t<CMP> cmp) noexcept
            {
                static_assert(CMP_LIST::  template contains<CMP>());

                auto& drawer = getDrawerOf<CMP>();
                auto user_key = drawer.push_back(std::move(cmp));

                return user_key;
            }


            private:

            template <typename T>
            using to_slotmap = zoje::static_slotmap<T,Capacity>;
            template <typename List>
            using to_tuple = zoje::ppUtils::replacer_t<std::tuple,List>;

            using storage_t = to_tuple<zoje::ppUtils::foreach_element_insert_t<to_slotmap,CMP_LIST>>;

            storage_t storage_{};

        };
    }
}

