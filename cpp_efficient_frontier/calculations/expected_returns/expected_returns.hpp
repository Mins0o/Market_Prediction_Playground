#pragma once

#include<vector>
#include "expected_returns_strategy.hpp"

class ArithmeticMeanStrategy:  public ExpectedReturnStrategy{
public:
    double get_expected_return(const std::vector<double>& returns_list);
};

class StatisticalStrategy: public ExpectedReturnStrategy{
public:
    double get_expected_return(const std::vector<double>& returns_list);
};
