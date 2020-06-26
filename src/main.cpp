#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>

// for convenience
using json = nlohmann::json;

int main() {
    // read a JSON file
    std::ifstream i("test.json");
    json j;
    i >> j;

    std::cout << std::setw(4) << j << std::endl;
}
