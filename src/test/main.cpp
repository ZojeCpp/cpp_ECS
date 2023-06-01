#include <ecs.hpp>
#include <iostream>
#include <typeinfo>


int main()

{
    short a{};
    int b{};
    long long c{};

    auto firstPackage = typename ppUtils::Package<int,int,int,int,int,int,int,int,int,int,int,int,float,int,int,int,int>{};
    auto secondPackage = typename  ppUtils::Package<int,int,int,int,int,int>{};
    auto thirdPackage = typename  ppUtils::Package<int,short,long,char,double,float>{};


    auto type = firstPackage.adjusted_type();
    auto type2 = secondPackage.adjusted_type(); 


    std::cout << "byte size of integer types used" << std::endl; 

    std::cout << sizeof(short) << std::endl;
    std::cout << sizeof(int) << std::endl;
    std::cout << sizeof(long long) << std::endl; 

    std::cout << "check type 1" << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(a)> << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(b)> << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(c)> << std::endl; 

    std::cout << "check type 2" << std::endl; 

    std::cout << std::is_same_v<decltype(type2),decltype(a)> << std::endl; 

    std::cout << std::is_same_v<decltype(type2),decltype(b)> << std::endl; 

    std::cout << std::is_same_v<decltype(type2),decltype(c)> << std::endl; 

    std::cout << "check type 1 vs type 2" << std::endl; 

    std::cout << std::is_same_v<decltype(type),decltype(type2)> << std::endl; 

    std::cout << "float position on type 1 on second package" << std::endl; 

    std::cout << firstPackage.position_of<float>() << std::endl;

    std::cout << "type on the position 4 on third package" << std::endl; 

    //THIS IS USED TO SHOWCASE IT WORKS, RESULT SHOULD BE COMPILER'S NAME FOR DOUBLE
    std::cout << typeid(thirdPackage.type_on_pos<4>()).name() << std::endl;

    using TupleofThirdPackage = ppUtils::replacer_t<std::tuple,decltype(thirdPackage)>;

    TupleofThirdPackage firstTuple{};

    return 0;
}