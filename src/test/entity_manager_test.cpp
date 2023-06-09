#include <ecs.hpp>
#include <iostream>
#include <vector>
#include <cassert>


int main()

{
    using CMP_LIST = zoje::Package<float,char,int,double>;
    using TAG_LIST = zoje::Package<float,char,int,double>;
    using UNIQ_CMP_LIST = zoje::Package<short>;



    zoje::EntityManager<CMP_LIST,TAG_LIST,UNIQ_CMP_LIST,100> EM{};

    auto& e = EM.createEntity();

    EM.addComponent<float>(e,float{3.0});

    double d = 1.0;

    EM.addComponent<double>(e,d);

    e.addTags<float,char,int>();

    auto& e2 = EM.createEntity();

    EM.addComponent<int>(e2,1);

    e.mark4destruction();

    EM.update();

    //AFTER UPDATE  "e" and "e2" have been unvalidated

    auto& uniq_cmp = EM.getUniqueComponent<short>();

    std::cout << uniq_cmp << std::endl;

    EM.initUniqueComponent<short>(10);

    std::cout << uniq_cmp << std::endl;

    return 0;
}