#include <map>
#include <vector>
#include <iostream>

struct OfxPropertySetStruct {
    std::map<std::string, std::vector<int>> integers;
    std::map<std::string, std::vector<double>> doubles;
    std::map<std::string, std::vector<char*>> strings;
    std::map<std::string, std::vector<void*>> pointers;

    OfxPropertySetStruct() {
        std::cout << "Constructor" << std::endl;
        integers = std::map<std::string, std::vector<int>>();
        doubles = std::map<std::string, std::vector<double>>();
        pointers = std::map<std::string, std::vector<void*>>();
    }

    ~OfxPropertySetStruct() {
        for (auto mapPair : strings) {
            for (auto element : mapPair.second) {
                delete[] element;
            }
        }
    }
};