#pragma once

#include <iostream>
#include <vector>

#include "../data_reading_sandbox/data_reading_sandbox.hpp"


namespace calculations{

class Calculations{
public: 
    static float NetReturn(std::vector<float> stock_column);
};

}
