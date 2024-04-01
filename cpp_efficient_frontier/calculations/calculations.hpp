#pragma once

#include <iostream>
#include <vector>

namespace calculations{

class Calculations{
public: 
    static float net_return(const std::vector<double>& returns);
    static void get_acc_return_list(const std::vector<double>& returns, std::vector<double>& values);
};

}
