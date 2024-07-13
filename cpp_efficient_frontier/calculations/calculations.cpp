#include "calculations.h"
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>
#include "expected_returns/expected_returns_strategy.h"

namespace calculations {

	ExpectedReturnStrategy* Calculations::expected_return_strategy=nullptr;

	double Calculations::CompoundReturnToValue(const std::vector<double>& returns){
		double net_value = 1.0;
		for(double return_value: returns){
			net_value *= 1 + (return_value/100);
		}
		return net_value;
	}

	std::vector<double> Calculations::CompoundReturnToValueSeries(/*I*/ const std::vector<double>& returns){
		double net_value = 1.0;
		std::vector<double> values={};
		values.reserve(returns.size());
		values.emplace_back(net_value);
		for(double return_value: returns){
			net_value *= 1 + (return_value/100);
			values.emplace_back(net_value);
		}
		return values;
	}

	std::vector<double> Calculations::CompoundPeriodicRebalancedSeries(/*I*/ const std::vector<double>& returns,
						/*I*/ const size_t rebalancing_term,
						/*O*/ std::vector<double>& compounded_values){
		double net_value = 1.0;
		std::vector<double> rebalanced_surplus = {};
		compounded_values = {};
		compounded_values.reserve(returns.size());
		compounded_values.emplace_back(net_value);
		for(int ii=0; ii<returns.size(); ii++){
			if(ii%rebalancing_term == 0){
				rebalanced_surplus.emplace_back(net_value-1);
				net_value = 1;
			}
			net_value *= 1 + (returns[ii]/100);
			compounded_values.emplace_back(net_value);
		}
		return rebalanced_surplus;
	}

	std::vector<double> Calculations::CompoundPeriodicRebalancedSeries(/*I*/ const std::vector<double>& returns,
						/*I*/ const std::vector<size_t>& rebalancing_indices,
						/*O*/ std::vector<double>& compounded_values){
		double net_value = 1.0;
		std::vector<double> rebalanced_surplus = {};
		compounded_values = {};
		compounded_values.reserve(returns.size());
		compounded_values.emplace_back(net_value);
		size_t rebalancing_index = 0;
		for(int ii=0; ii<returns.size(); ii++){
			if(rebalancing_index < rebalancing_indices.size() && ii == rebalancing_indices[rebalancing_index]){
				rebalanced_surplus.emplace_back(net_value-1);
				net_value = 1;
				rebalancing_index++;
			}
			net_value *= 1 + (returns[ii]/100);
			compounded_values.emplace_back(net_value);
		}
		return rebalanced_surplus;
	}

	std::vector<double> Calculations::CalculateReturnsFromValueSeries(/*I*/ const std::vector<double>& stripped_values){
		double prev = stripped_values[0];
		std::vector<double> change_rates = {};
		change_rates.reserve(stripped_values.size());
		for (double value: stripped_values){
			change_rates.emplace_back((value/prev - 1)*100);
			prev = value;
		}
		return change_rates;
	}

	double Calculations::Average(const std::vector<double>& returns){
		if (returns.empty()){
			return 0;
		}
		return std::reduce(returns.begin(), returns.end())/returns.size();
	}

	double Calculations::StandardDeviation(const std::vector<double>& returns){
		double average_return = Calculations::Average(returns);
		double accumulate = 0;
		for(double return_v : returns){
			accumulate += (return_v-average_return) * (return_v-average_return);
		}
		return std::sqrt(accumulate / returns.size());
	}

	std::vector<double> Calculations::WeightedSumOfTwoSeries(/*I*/ const std::vector<double>& values_a, 
							/*I*/ const double weight_a,
							/*I*/ const std::vector<double>& values_b,
							/*I*/ const double weight_b){
		std::vector<double> result = {};
		result.reserve(values_a.size());

		if (values_a.size() != values_b.size()){
			return result;
		}

		for (int ii = 0; ii < values_a.size(); ii++){
			result.emplace_back(values_a[ii]*weight_a + values_b[ii]*weight_b);
		}
		return result;
	}

	std::vector<double> Calculations::WeightedSumOfSeries(/*I*/ const std::vector<std::vector<double>>& columns_of_values,
							/*I*/ const std::vector<double>& weights){
		if(weights.empty() || columns_of_values.empty()){
			std::cout << "Calculations::weighted_sum2: empty vectors" << std::endl;
		}

		size_t number_of_securities = columns_of_values.size();
		std::vector<double> results(columns_of_values[0].size(), 0);

		for (int ii=0; ii<number_of_securities; ii++){
			auto& values = columns_of_values[ii];
			for (int jj=0; jj<results.size(); jj++){
				results[jj] += values[jj]*weights[ii];
			}
		}

		return results;
	}

	std::vector<double> Calculations::WeightedSumOfValuesFromReturns(/*I*/ const std::vector<std::vector<double>>& returns,
								/*I*/ const std::vector<double>& weights){
		if(weights.empty() || returns.empty()){
			std::cout << "Calculations::weighted_sum: empty vectors" << std::endl;
		}
		std::vector<double> mixed_values;
		std::vector<double> mixed_returns;
		size_t number_of_securities = returns.size();
		std::vector<std::vector<double>> compounded = {};

		for (const auto& ret : returns){
			compounded.emplace_back(CompoundReturnToValueSeries(ret));
		}
		mixed_values = WeightedSumOfSeries(compounded, weights);
		mixed_returns = CalculateReturnsFromValueSeries(mixed_values);

		return mixed_returns;
	}

	std::vector<double> Calculations::WeightedSumOfValuesWithRebalancing(/*I*/ const std::vector<std::vector<double>>& returns,
								/*I*/ const std::vector<double>& weights,
								/*I*/ const size_t rebalancing_term){
		if(weights.empty() || returns.empty()){
			std::cout << "Calculations::rebalanced_weighted_sum: empty vectors" << std::endl;
		}

		size_t number_of_securities = returns.size();
		std::vector<double> result_values = {1};
		std::vector<std::vector<double>> compounded(weights.size(),std::vector<double>());

		for (int mm=0; mm<weights.size(); mm++){
			compounded[mm].emplace_back(weights[mm]);
		}

		for(int ii=0; ii<returns[0].size(); ii++){
			double rebalanced_total = 0;
			for (int jj=0; jj<weights.size(); jj++){
				compounded[jj].emplace_back(compounded[jj][ii] * (1 + returns[jj][ii]/100.0));
				rebalanced_total += compounded[jj][ii+1];
			}
			if (rebalancing_term!=0 && ii%rebalancing_term==0){
				for (int ll=0; ll<returns.size(); ll ++){
					compounded[ll][ii+1] = rebalanced_total * weights[ll];
				}
			}
			result_values.emplace_back(rebalanced_total);
		}
		return result_values;
	}

	std::vector<double> Calculations::WeightedSumOfValuesWithRebalancing(/*I*/ const std::vector<std::vector<double>>& returns,
								/*I*/ const std::vector<double>& weights,
								/*I*/ const std::vector<size_t>& rebalancing_indices){
		if(weights.empty() || returns.empty()){
			std::cout << "Calculations::rebalanced_weighted_sum: empty vectors" << std::endl;
		}

		size_t number_of_securities = returns.size();
		std::vector<double> result_values = {1};
		std::vector<std::vector<double>> compounded(weights.size(),std::vector<double>());

		for (int mm=0; mm<weights.size(); mm++){
			compounded[mm].emplace_back(weights[mm]);
		}

		size_t rebalancing_index = 0;
		for(int ii=0; ii<returns[0].size(); ii++){
			double rebalanced_total = 0;
			for (int jj=0; jj<weights.size(); jj++){
				compounded[jj].emplace_back(compounded[jj][ii] * (1 + returns[jj][ii]/100.0));
				rebalanced_total += compounded[jj][ii+1];
			}
			if (rebalancing_index < rebalancing_indices.size() && ii == rebalancing_indices[rebalancing_index]){
				for (int ll=0; ll<returns.size(); ll ++){
					compounded[ll][ii+1] = rebalanced_total * weights[ll];
				}
				rebalancing_index++;
			}
			result_values.emplace_back(rebalanced_total);
		}
		return result_values;
	}

	std::vector<double> Calculations::ChunkReturns(/*I*/ const std::vector<double>& trimmed_daily_returns,
									/*I*/ size_t intended_period){
		double current_value = 1.0;
		auto current_return_it = trimmed_daily_returns.begin();
		std::vector<double> chunked ={};
		size_t chunked_array_length = trimmed_daily_returns.size()/intended_period;
		chunked.reserve(chunked_array_length);

		bool is_end_reached = false;
		while(not is_end_reached){
			chunked.emplace_back((current_value-1)*100);
			current_value = 1.0;

			for (int ii=0; ii<intended_period; ii++){
				if (current_return_it == trimmed_daily_returns.end()){
					is_end_reached = true;
					break;
				}
				current_value *= (1 + 0.01*(*current_return_it));
				current_return_it++;
			}
		}

		return chunked;
	}

	void Calculations::SetExpectedReturnStrategy(/*I*/ ExpectedReturnStrategy* strategy){
		Calculations::expected_return_strategy = strategy;
	}

	double Calculations::GetExpectedReturn(/*I*/ const std::vector<double>& returns){
		if (!expected_return_strategy){
			std::cout << "Calculations::GetExpectedReturn: No strategy has been set.\n returning 0"
				<< std::endl;
			return 0;
		}
		return expected_return_strategy->GetExpectedReturn(returns);
	}
}
