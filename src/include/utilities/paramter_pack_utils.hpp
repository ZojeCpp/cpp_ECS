#pragma once

#include <type_traits>
#include <cstdint>

//pp short for parameter pack
namespace ppUtils
{


    //MY OWN IMPLEMENTATION OF is_same THAT DOESNT REQUIRE EXPLICIT TEMPLATE PRARMETER DECLARATION
    /// @brief MY OWN IMPLEMENTATION OF is_same THAT DOESNT REQUIRE EXPLICIT TEMPLATE PRARMETER DECLARATION, DEPRACTED SINCE STD'S IS BETTER
    /// @tparam T 
    /// @param lhs 
    /// @param rhs 
    /// @return true if both variables are of the same type, otherwise false
    template<typename T>
    static constexpr bool  is_same(T lhs, T rhs)
    {
        return true;
    }

    template<typename T, typename U>
    static constexpr bool is_same(T lhs, U rhs)
    {
        return false;
    }


    //////////////////////////////////////////////////////

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
    /// @return depends on prameter pack size
    template<typename... Ts>
    struct smallest_mask_type_size_t{ using adjusted_type = typename if_t < typename if_t <typename if_t<uint8_t,uint16_t,Ts...>::type,uint32_t,Ts...>::type,uint64_t,Ts...>::type; };
    //////////////////////////////////////////////////////


    /// @brief return struct that contains position of the first instance of type T on the prameter pack
    /// @tparam T type whose position we are searching for
    /// @tparam ...Ts 
    template<typename T, typename... Ts>
    struct pos_type {static_assert(sizeof...(Ts)!=0);};
    
    /// @brief return position of the first instance of type T on the prameter pack, helper so that user doesn'thave to call ::value
    /// @tparam T type whose position we are searching for
    /// @tparam ...Ts 
    template<typename T, typename...Ts>
    constexpr auto pos_type_v = pos_type<T,Ts...>::value; 

    template<typename T, typename... Ts>
    struct pos_type<T,T,Ts...> : std::integral_constant<std::size_t,0>{}; 

    template<typename T,typename U, typename... Ts>
    struct pos_type<T,U,Ts...> : std::integral_constant<std::size_t,1 + pos_type_v<T,Ts...>>{};



    //////////////////////////////////////////////////////



    /// @brief returns the type on N position in the parameter pack
    /// @tparam ...Ts 
    /// @tparam N 
    template<std::size_t N, typename... Ts>
    struct nth_type{static_assert(sizeof...(Ts)!=0);};

    /// @brief returns the type on N position in the parameter pack, helper so that user doesn'thave to call ::type
    /// @tparam ...Ts 
    /// @tparam N 
    template<std::size_t N, typename... Ts>
    using nth_type_t = typename nth_type<N,Ts...>::type;

    template<typename T, typename... Ts>
    struct nth_type<0,T,Ts...> { using type = T; };

    //THIS SPECIALIZATION CALLS THE HELPER WHO CALLS THIS FUNCTION AGAIN UNTIL N==0 THEN IT BREAKS RECURSIVE LOOP 
    //AND ENTERS N==0 SPECIALZATION AND RETURNS FIRST TYPE ON REMAINING PARAMTER PACK
    template<std::size_t N,typename T, typename... Ts>
    struct nth_type<N,T,Ts...> { using type = nth_type_t<N-1,Ts...>; };

    //////////////////////////////////////////////////////

} // namespace typePack 
