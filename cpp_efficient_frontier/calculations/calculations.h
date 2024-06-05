#pragma once
#ifndef EFFICIENTFRONTIER_CALCULATIONS_CALCULATIONS_H_
#define EFFICIENTFRONTIER_CALCULATIONS_CALCULATIONS_H_

#include <iostream>
#include <vector>
#include "expected_returns/expected_returns_strategy.h"

namespace calculations{

class Calculations{
public:
	static ExpectedReturnStrategy* expected_return_strategy;
	/**
	 * calculates the end value assuming value of 1 at the start of the vector
	 * @param returns vector of periodic returns on the security
	 */
	static double net_value(/*I*/ const std::vector<double>& returns);

	/**
	 * creates a vector of compounded values at times that compiles the periodic returns
	 * @param returns vector of periodic returns on the security
	 * @return compounded value of the returns, assuming the starting value is 1
	 */
	static std::vector<double> value_series(/*I*/ const std::vector<double>& returns);

	/**
	* with daily returns and rebalancing period, compound the returns into values.
	* the return/loss during rebalancing is returned
	* @param returns periodic return, most likely daily
	* @param rebalancing_term once in `rebalancing_term` the returns are rebalanced
	* @param compounded_values compounded value of the returns
	* @return the surplus return/loss is returned in vector<double>
	*/
	static std::vector<double> rebalance_compound(/*I*/ const std::vector<double>& returns,
							/*I*/ const size_t rebalancing_term,
							/*O*/ std::vector<double>& compounded_values);

	/**
	 * creates a vector of change rates from a vector of compounded values.
	 * @param stripped_value timeseries of values of a security. assumes there are no 0 value
	 * @return change rates calculated from the compounded valueinput
	 */
	static std::vector<double> values_to_change(/*I*/ const std::vector<double>& stripped_values);

	/**
	 * calulates the average of the periodic returns
	 * @param returns vector of periodic returns on the security
	 */
	static double average(/*I*/ const std::vector<double>& returns);

	/**
	 * calculates the standard deviation of the periodic returns
	 * @param returns vector of periodic returns on the security
	 */
	static double standard_deviation(/*I*/ const std::vector<double>& returns);

	/**
	 * calculates an expected return for the next single period in the time series based on the given historical record
	 */
	static double expected_return(const std::vector<double>& returns);
	static double expected_return_avg(const std::vector<double>& returns);
	static double expected_return_histogram(const std::vector<double>& returns, double bin_size);

	/**
	 * returns a vector that combines the two given vectors with the given weights
	 * the weights do not need to sum up to 1
	 * the length of the two vectors should match
	 * @param values_x 
	 * @param weight_x 
	 */
	static std::vector<double> weighted_sum(/*I*/ const std::vector<double>& values_a, 
						/*I*/ const double weight_a,
						/*I*/ const std::vector<double>& values_b,
						/*I*/ const double weight_b);

	/**
	 * returns a vector that combines a list vectors with the given weights
	 * the weights do not need to sum up to 1
	 * the length of the two vectors should match
	 * @param columns_of_values 
	 * @param weights 
	 */
	static std::vector<double> weighted_sum(/*I*/ const std::vector<std::vector<double>>& columns_of_values,
						/*I*/ const std::vector<double>& weights);

	/**
	 * Calculates the rebalanced weighted sum of returns based on the given weights and rebalancing term.
	 *
	 * @param returns The vector of vectors representing the returns of different assets over time.
	 * @param weights The vector of weights assigned to each asset.
	 * @param rebalancing_term The size of the rebalancing term.
	 * @return The rebalanced weighted sum of returns.
	 */
	std::vector<double> rebalanced_weighted_sum(/*I*/ const std::vector<std::vector<double>>& returns,
								/*I*/ const std::vector<double>& weights,
								/*I*/ const size_t rebalancing_term);

	/**
	 * compounds daily returns into periodic return of a longer term.
	 * For now, trimmed daily return is given and intended period is applied simply by the number of entries.
	 * 
	 */
	static std::vector<double> aggregate_returns_by_period(/*I*/ const std::vector<double>& trimmed_daily_returns,
								/*I*/ size_t intended_period);


	static void set_expected_return_strategy(/*I*/ ExpectedReturnStrategy*);
	static double get_expected_return(/*I*/ const std::vector<double>& returns);
};

}

#endif // EFFICIENTFRONTIER_CALCULATIONS_CALCULATIONS_H_