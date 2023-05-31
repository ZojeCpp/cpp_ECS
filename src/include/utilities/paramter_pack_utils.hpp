#pragma once

#include <type_traits>

//pp short for parameter pack
namespace ppUtils
{

    // MY OWN IMPLEMENTATION OF is_same THAT DOESNT REQUIRE EXPLICIT TEMPLATE PRARMETER DECLARATION
    // /// @brief 
    // /// @tparam T 
    // /// @param lhs 
    // /// @param rhs 
    // /// @return 
    // template<typename T>
    // static constexpr bool  is_same(T lhs, T rhs)
    // {
    //     return true;
    // }

    // template<typename T, typename U>
    // static constexpr bool is_same(T lhs, U rhs)
    // {
    //     return false;
    // }


    static constexpr short byteSize = 8;

    /// @brief returns the size of T in bits instead of bytes 
    /// @tparam T 
    template <typename T>
    struct bit_sizeof{ static constexpr auto value = sizeof(T)*byteSize;};

    /// @brief specialized if on compilation time to get one of two types based on if result
    /// @tparam U first type
    /// @tparam F second type
    /// @tparam ...T parameter pack whose size decides if result
    template<typename U,typename F , typename... Ts>
    struct if_t{ using type = typename std::conditional<(sizeof...(Ts)<=bit_sizeof<U>::value),U,F>::type; }; 

    /// @brief change return type based on parameter pack size, used when type size needs to be optimized
    /// @tparam ...T 
    /// @return depends on prameter pack size return type can be: short, int, long long
    template<typename... Ts>
    consteval static auto adjust_type_size()
    {
        using retValue = typename if_t <typename if_t<short,int,Ts...>::type,long long,Ts...>::type;

        return retValue{};
    }

    /// @brief return struct that contains position of the first instance of type T on the prameter pack
    /// @tparam T type whose position we are searching for
    /// @tparam ...Ts 
    template<typename T, typename... Ts>
    struct pos_type {static_assert(sizeof...(Ts)!=0);};
    
    /// @brief return position of the first instance of type T on the prameter pack
    /// @tparam T type whose position we are searching for
    /// @tparam ...Ts 
    template<typename T, typename...Ts>
    constexpr auto pos_type_v = pos_type<T,Ts...>::value; 
    template<typename T, typename... Ts>
    struct pos_type<T,T,Ts...> : std::integral_constant<std::size_t,0>{}; 

    template<typename T,typename U, typename... Ts>
    struct pos_type<T,U,Ts...> : std::integral_constant<std::size_t,1 + pos_type_v<T,Ts...>>{};



}
