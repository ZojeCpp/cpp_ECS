#pragma once
#include <include/utilities/slotmap.hpp>
#include <include/utilities/type_pack.hpp>
#include <cstdint>


namespace zoje 
{
    namespace cmp
    {
        template<typename CMP_LIST,typename TAG_LIST,typename UNIQUE_CMP_LIST, size_t Capacity = 10>
        struct storer
        {
            using cmp_info = CMP_LIST;
            using tag_info = TAG_LIST;

            template<typename T>
            using to_key_type    = typename zoje::static_slotmap<T,Capacity>::key_type;

            template<typename UNIQ_CMP>
            constexpr void insertUnique(ppUtils::r_reference_or_l_reference_t<UNIQ_CMP> uniq_cmp) noexcept
            {
                static_assert(UNIQUE_CMP_LIST::  template contains<UNIQ_CMP>());

                auto& container = getUnique<UNIQ_CMP>();
                container = std::move(uniq_cmp);
            }

            template<typename UNIQ_CMP>
            [[__nodiscard__]] constexpr auto& getUnique() noexcept
            {
                static_assert(UNIQUE_CMP_LIST::  template contains<UNIQ_CMP>());
                return std::get<UNIQUE_CMP_LIST:: template position_of<UNIQ_CMP>()>(uniq_storage_);
            }

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

            using cmp_storage_t = to_tuple<zoje::ppUtils::foreach_element_insert_t<to_slotmap,CMP_LIST>>;

            using uniq_cmp_storage_t = to_tuple<UNIQUE_CMP_LIST>;


            cmp_storage_t storage_{};
            uniq_cmp_storage_t uniq_storage_{};

        };
    }
}

