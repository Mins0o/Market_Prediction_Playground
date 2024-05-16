#pragma once

#include<vector>
#include "expected_returns_strategy.hpp"

class ArithmeticMeanStrategy:  public ExpectedReturnStrategy{
public:
    double get_expected_return(const std::vector<double>& returns_list);
};

class DiscountedMeanStrategy: public ExpectedReturnStrategy{
private:
    double discount_rate_ = 0;
public:
    DiscountedMeanStrategy();
    DiscountedMeanStrategy(double decay_rate);
    double get_expected_return(const std::vector<double>& returns_list);
};

