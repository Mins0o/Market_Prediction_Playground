#include <numeric>
#include <vector>
#include <cmath>
#include "expected_returns.hpp"

double ArithmeticMeanStrategy::get_expected_return(/*I*/ const std::vector<double>& returns_list){
	if (returns_list.empty()){
		return 0.0;
	}
	return std::reduce(returns_list.begin(), returns_list.end())/returns_list.size();
}

DiscountedMeanStrategy::DiscountedMeanStrategy(){
	discount_rate_ = 0;
}

/**
 * @discount_rate bigger rate ignores the past values more strongly. value x means the last element is weighted 10^-x
 */
DiscountedMeanStrategy::DiscountedMeanStrategy(double dicount_rate){
						discount_rate_ = discount_rate_;
}

double DiscountedMeanStrategy::get_expected_return(const std::vector<double>& returns_list){
	if (returns_list.empty()){
		return 0.0;
	}
	if (returns_list.size()==1){
		return returns_list[0];
	}

	size_t array_size = returns_list.size();
	double weight_sum = 0;
	double weighted_sum = 0;

	for (int ii = 0; ii<array_size; ii++){
		double power_value = -(ii+0.0)/(array_size-1);
		double weight_value = std::pow(10, power_value * discount_rate_);
		weighted_sum += returns_list[array_size-ii-1]*weight_value;
		weight_sum += weight_value;
	}

	return weighted_sum/weight_sum;
}
