#include <ecs.hpp>
#include <iostream>
#include <vector>
#include <cassert>


int main()

{
    using CMP_LIST = zoje::Package<float,char,int,double>;
    using TAG_LIST = zoje::Package<float,char,int,double>;


    zoje::EntityManager<CMP_LIST,TAG_LIST> EM{};

    auto& e = EM.createEntity();

    EM.addComponent<float>(e,float{3.0});

    double d = 1.0;

    EM.addComponent<double>(e,d);

    e.addTags<float,char,int>();

    auto& e2 = EM.createEntity();

    //auto& cmp = EM.getComponent<double>(e);

    e.mark4destruction();

    EM.update();

    return 0;
}