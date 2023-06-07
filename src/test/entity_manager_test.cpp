#include <ecs.hpp>
#include <iostream>
#include <vector>
#include <typeinfo>


int main()

{
    using CMP_LIST = zoje::Package<float,char,int,double>;

    zoje::EntityManager<CMP_LIST> EM{};


    return 0;
}