//
// Created by Bill Fu on 2020/5/12.
//

#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using nlohmann::json;


int main()
{
    //cout << "Hello, World!";

    // create an empty structure (null)
    json j;

    // add a number that is stored as double (note the implicit conversion of j to an object)
    j["pi"] = 3.141;

    // add a Boolean that is stored as bool
    j["happy"] = true;

    // add a string that is stored as std::string
    j["name"] = "Niels";

    // add another null object by passing nullptr
    j["nothing"] = nullptr;

    // add an object inside the object
    j["answer"]["everything"] = 42;

    // add an array that is stored as std::vector (using an initializer list)
    j["list"] = { 1, 0, 2 };

    // add another object (using an initializer list of pairs)
    j["object"] = { {"currency", "USD"}, {"value", 42.99} };

    // explicit conversion to string
    //std::string s = j.dump();    // {"happy":true,"pi":3.141}

    // serialization with pretty printing
    // pass in the amount of spaces to indent
    std::cout << j.dump(4) << std::endl;

    return 0;
}

