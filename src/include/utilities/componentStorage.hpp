#pragma once
#include <include/utilities/slotmap.hpp>
#include <include/utilities/type_pack.hpp>
#include <cstdint>


namespace cmp 
{
    template<typename CMP_LIST, size_t Capacity = 10>
    struct storer
    {
        using cmp_info = CMP_LIST;
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



        private:

        template <typename T>
        using to_slotmap = zoje::static_slotmap<T,Capacity>;
        template <typename List>
        using to_tuple = ppUtils::replacer_t<std::tuple,List>;

        using storage_t = to_tuple<ppUtils::foreach_element_insert_t<to_slotmap,CMP_LIST>>;

        storage_t storage_{};

    };
}

