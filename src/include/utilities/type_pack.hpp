#pragma once

#include <include/utilities/paramter_pack_utils.hpp>


//pp short for parameter pack
namespace ppUtils
{
    /// @brief Struct defined for the comfortable use of prameter packs
    /// @tparam ...Ts parameter pack contained by the TypePack
    template<typename... Ts>
    struct Package{
        //static_assert(size() <= 64, "Error: Maximum of 64 elements allowed");
        
        private : struct mask_type { using type = ppUtils::smallest_mask_type_size_t<Ts...>::adjusted_type; };
        
        public :
        // Package()
        // {
        // }

        /// @brief return the size of the package
        /// @return 
        [[__nodiscard__]] consteval static auto size()
        {
            return sizeof...(Ts);;
        }

        /// @brief checks if T is part of the prameter pack
        /// @tparam T 
        /// @return true if T it's found, false otherwise
        template<typename T>
        [[__nodiscard__]] consteval static auto contains()
        {
            return (false || ... || std::is_same_v<T,Ts>);
        }

        template <std::size_t pos>
        [[__nodiscard__]] consteval static auto type_on_pos()
        {
            return ppUtils::nth_type_t<pos,Ts...>{};
        }

        /// @brief returns the position of T in the Package
        /// @tparam T 
        /// @return 
        template<typename T>
        [[__nodiscard__]] consteval static uint8_t position_of()
        {
            static_assert(contains<T>());
            return pos_type_v<T,Ts...>;
        }


        using mask_type_t = typename mask_type::type;
        
        template<typename T>
        consteval static mask_type_t mask() noexcept
        {
            return 1 << position_of<T>();
        }

    };

    template< template <typename...> class newContainer ,typename Package>
    struct replacer {};

    template< template <typename...> class newContainer  ,typename... Ts>
    struct replacer<newContainer,Package<Ts...>> { using type = newContainer<Ts...>;};

    template< template <typename...> class newContainer  ,typename Package>
    using replacer_t = typename replacer<newContainer,Package>::type;


    template<template <typename... > class NewContainer, typename  List >
    struct foreach_element_insert{};

    template<template <typename... > class NewContainer, typename... Ts >
    struct foreach_element_insert<NewContainer,Package<Ts...>>{ using type = Package<NewContainer<Ts>...>;};


    template<template <typename... > class NewContainer, typename  List >
    using foreach_element_insert_t = typename foreach_element_insert<NewContainer, List>::type;

} // namespace typePack

