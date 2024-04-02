#pragma once

#include <iostream>
#include <vector>

namespace calculations{

class Calculations{
public: 
    static double net_return(const std::vector<double>& returns);
    static std::vector<double>& value_series(const std::vector<double>& returns, std::vector<double>& values);
    static double average(const std::vector<double>& returns);
    static double standard_deviation(const std::vector<double>& returns);
    static double expected_return(const std::vector<double>& returns);
    static double weighted_sum(const std::vector<double>& returns_a, const std::vector<double>& returns_b);
};

}
