#ifndef EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNS_H_
#define EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNS_H_

#include<vector>
#include "expected_returns_strategy.h"

/*
Strategy pattern
*/

class ArithmeticMeanStrategy:  public ExpectedReturnStrategy{
public:
	double GetExpectedReturn(const std::vector<double>& returns_list);
};

class DiscountedMeanStrategy: public ExpectedReturnStrategy{
private:
	double discount_rate_ = 0;
public:
	DiscountedMeanStrategy() : discount_rate_(0) {};
	/**
 	 * @discount_rate bigger rate ignores the past values more strongly. value x means the last element is weighted 10^-x
 	 */
	DiscountedMeanStrategy(double decay_rate): discount_rate_(decay_rate){};
	double GetExpectedReturn(const std::vector<double>& returns_list);
};
#endif // EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNS_H_