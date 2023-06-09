#include <include/utilities/slotmap.hpp>
#include <iostream>
#include <string>


struct someData
{
    std::string name{};
    int number{};
};

void listSlotmap(auto slotmap)
{
    std::cout << "Current Storage state:" << std::endl;

    for(auto data : slotmap)
    {
        std::cout << data.name << ", " << data.number << std::endl;
    }

    std::cout << "//////////////////////////" << std::endl;

}

//TODO
//IMPLEMENT PROFILING TO SHOWCASE DIFFERENCE BETWEEN STATIC AND DYNAMIC SLOTMAP
//ENUMMERATE ADVANTAGES AND DISADVANTAGES OF BOTH

int main()
{
    
    zoje::slotmap<someData> dataStorage{};
    zoje::static_slotmap<someData,10> static_dataStorage{};


    auto firstKey = dataStorage.push_back(someData{"first", 2});

    auto secondkey = dataStorage.push_back(someData{"first", 4});

    someData localData = {"second", 2};

    auto thirdKey = dataStorage.push_back(std::move(localData));

    listSlotmap(dataStorage);

    dataStorage.erase(firstKey);

    listSlotmap(dataStorage);

    dataStorage.erase(thirdKey);

    listSlotmap(dataStorage);
    
   firstKey = dataStorage.push_back(someData{"third", 2});

   thirdKey = dataStorage.push_back(someData{"third", 3});

    listSlotmap(dataStorage);

    dataStorage.erase(secondkey);

    listSlotmap(dataStorage);

    dataStorage.erase(firstKey);
    dataStorage.erase(thirdKey);

    listSlotmap(dataStorage);

    return 0;
}