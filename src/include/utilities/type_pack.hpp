#pragma once

#include <include/utilities/paramter_pack_utils.hpp>


namespace ppUtils
{
    /// @brief Struct defined for the comfortable use of prameter packs
    /// @tparam ...Ts parameter pack contained by the TypePack
    template<typename... Ts>
    struct Package{

        /// @brief return the size of the package
        /// @return 
        consteval static auto size()
        {
            return sizeof...(Ts);;
        }

        /// @brief checks if T is part of the prameter pack
        /// @tparam T 
        /// @return true if T it's found, false otherwise
        template<typename T>
        consteval static auto contains()
        {
            return (false || ... || std::is_same_v<T,Ts>);
        }

        
        consteval static auto adjusted_type()
        {
           return ppUtils::adjust_type_size<Ts...>();
        }

        /// @brief returns the position of T in the Package
        /// @tparam T 
        /// @return 
        template<typename T>
        consteval static auto position_of()
        {
            static_assert(contains<T>());
            return pos_type_v<T,Ts...>;
        }

    };
} // namespace typePack

