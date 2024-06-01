#pragma once
#ifndef EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNS_H_
#define EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNS_H_

#include<vector>
#include "expected_returns_strategy.h"

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
#endif // EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNS_H_