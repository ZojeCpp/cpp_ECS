#include <ecs.hpp>
#include <iostream>
#include <vector>
#include <typeinfo>


int main()

{

    using firstPackage = typename  ppUtils::Package<int,short,long,char,double,float>;

    cmp::storer<firstPackage> firstStorer{};

    

    //std::cout << firstStorer.getId<double>() << std::endl;

    unsigned int i =  size_t(cmp::storer<firstPackage>::cmp_info::position_of<double>());

    std::cout << i << std::endl;

    i =  cmp::storer<firstPackage>::cmp_info::mask<double>();

    std::cout << i << std::endl;


    auto& topDrawer = firstStorer.getDrawerOf<double>();

    auto key = topDrawer.push_back(2.0);

    std::cout << key.first << "," << key.second << std::endl;
    

    return 0;
}