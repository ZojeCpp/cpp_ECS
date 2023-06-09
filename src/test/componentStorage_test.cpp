#include <ecs.hpp>
#include <iostream>
#include <vector>
#include <typeinfo>


int main()

{

    using firstPackage = typename  zoje::Package<int,short,long,double,float>;

    using storer_t  = zoje::cmp::storer<firstPackage,zoje::Package<>,zoje::Package<char>,10>;

    storer_t firstStorer{};

    

    //std::cout << firstStorer.getId<double>() << std::endl;

    unsigned int i =  size_t(storer_t::cmp_info::position_of<double>());

    std::cout << i << std::endl;

    i =  storer_t::cmp_info::mask<double>();

    std::cout << i << std::endl;

    auto firstKey = firstStorer.insert<double>(1.0);

    auto secondKey = firstStorer.insert<double>(double{2.0});

    double d{3.0};

    auto thirdKey = firstStorer.insert<double>(d);

    auto initialDrawer = firstStorer.getDrawerOf<double>();

    for(auto& dbl : initialDrawer)
    {
        std::cout << dbl << std::endl;
    }
    

    d = firstStorer.getDataByKey<double>(secondKey);

    std::cout << d << std::endl;


    firstStorer.pop<double>(secondKey);

    firstStorer.pop<double>(firstKey);

    firstStorer.pop<double>(thirdKey);

    auto finalDrawer = firstStorer.getDrawerOf<double>();

    for(auto& dbl : finalDrawer)
    {
        std::cout << dbl << std::endl;
    }

    firstStorer.insert<char>({'c'});

    auto& c = firstStorer.getUnique<char>();

    std::cout << c << std::endl;

    firstStorer.insert<char>({'d'});

    std::cout << c << std::endl;

    return 0;
}