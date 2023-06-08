#pragma once
#include <include/utilities/slotmap.hpp>
#include <include/utilities/type_pack.hpp>
#include <cstdint>


namespace zoje 
{
    namespace cmp
    {
        template<typename CMP_LIST,typename TAG_LIST, size_t Capacity = 10>
        struct storer
        {
            using cmp_info = CMP_LIST;
            using tag_info = TAG_LIST;

            template<typename T>
            using to_key_type    = typename zoje::static_slotmap<T,Capacity>::key_type;

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

            template<typename CMP>
             constexpr void pop(auto key) noexcept
            {
                static_assert(CMP_LIST::  template contains<CMP>());

                auto& drawer = getDrawerOf<CMP>();
                drawer.erase(key);
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

