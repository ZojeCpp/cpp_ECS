#include <ecs.hpp>
#include <iostream>
#include <vector>
#include <typeinfo>


int main()

{
    uint8_t a{};
    uint16_t b{};
    uint32_t c{};
    uint64_t d{};

    using firstPackage = typename ppUtils::Package<uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,float,uint16_t,uint16_t,uint16_t,uint16_t>;
    using secondPackage = typename  ppUtils::Package<uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t>;
    using thirdPackage = typename  ppUtils::Package<uint16_t,uint8_t,uint32_t,char,double,float>;

    firstPackage::mask_type::type type{};
    secondPackage::mask_type::type type2{}; 

    std::cout << "byte size of integer types used" << std::endl; 

    std::cout << sizeof(uint8_t) << std::endl;
    std::cout << sizeof(uint16_t) << std::endl;
    std::cout << sizeof(uint32_t) << std::endl;
    std::cout << sizeof(uint64_t) << std::endl; 

    std::cout << "check type 1" << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(a)> << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(b)> << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(c)> << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(d)> << std::endl; 

    std::cout << "check type 2" << std::endl; 

    std::cout << std::is_same_v<decltype(type2),decltype(a)> << std::endl; 

    std::cout << std::is_same_v<decltype(type2),decltype(b)> << std::endl; 

    std::cout << std::is_same_v<decltype(type2),decltype(c)> << std::endl; 

    std::cout << std::is_same_v<decltype(type2),decltype(d)> << std::endl; 

    std::cout << "check type 1 vs type 2" << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(type2)> << std::endl; 

    std::cout << "float position on type 1 on second package" << std::endl; 

    std::cout << unsigned int(firstPackage::position_of<float>()) << std::endl;

    std::cout << "type on the position 4 on third package" << std::endl; 

    //THIS IS USED TO SHOWCASE IT WORKS, RESULT SHOULD BE COMPILER'S NAME FOR DOUBLE
    std::cout << typeid(thirdPackage::type_on_pos<4>()).name() << std::endl;

    std::cout << thirdPackage::mask<double>() << std::endl;
    

    using vectorOfThirdPackage = ppUtils::foreach_element_insert_t<std::vector,thirdPackage>;

    using TupleofvectorOfThirdPackage = ppUtils::replacer_t<std::tuple,vectorOfThirdPackage>;

    TupleofvectorOfThirdPackage firstTuple{};

    return 0;
}