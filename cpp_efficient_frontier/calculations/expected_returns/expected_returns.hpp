#pragma once

#include<vector>
#include "expected_returns_strategy.hpp"

class ArithmeticMeanStrategy:  public ExpectedReturnStrategy{
public:
    double get_expected_return(std::vector<double> returns_list);
};