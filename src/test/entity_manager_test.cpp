#include <ecs.hpp>
#include <iostream>
#include <vector>
#include <typeinfo>


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

    return 0;
}