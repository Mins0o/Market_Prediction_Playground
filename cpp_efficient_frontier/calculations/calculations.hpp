#pragma once

#include <iostream>
#include <vector>

#include "../data_reading_sandbox/data_reading_sandbox.hpp"


namespace calculations{

class Calculations{
public: 
    static float NetReturn(const std::vector<float>& returns);
    static void Returns2Values(const std::vector<float>& returns, std::vector<float>& values);
};

}
