#include "calculations.h"
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>
#include "expected_returns/expected_returns_strategy.h"

namespace calculations {

	ExpectedReturnStrategy* Calculations::expected_return_strategy=nullptr;

	double Calculations::net_value(const std::vector<double>& returns){
		double net_value = 1.0;
		for(double return_value: returns){
			net_value *= 1 + (return_value/100);
		}
		return net_value;
	}

	std::vector<double> Calculations::value_series(/*I*/ const std::vector<double>& returns){
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

	std::vector<double> Calculations::rebalance_compound(/*I*/ const std::vector<double>& returns,
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

	std::vector<double> Calculations::values_to_change(/*I*/ const std::vector<double>& stripped_values){
		double prev = stripped_values[0];
		std::vector<double> change_rates = {};
		change_rates.reserve(stripped_values.size());
		for (double value: stripped_values){
			change_rates.emplace_back((value/prev - 1)*100);
			prev = value;
		}
		return change_rates;
	}

	double Calculations::average(const std::vector<double>& returns){
		if (returns.empty()){
			return 0;
		}
		return std::reduce(returns.begin(), returns.end())/returns.size();
	}

	double Calculations::standard_deviation(const std::vector<double>& returns){
		double average_return = Calculations::average(returns);
		double accumulate = 0;
		for(double return_v : returns){
			accumulate += (return_v-average_return) * (return_v-average_return);
		}
		return std::sqrt(accumulate / returns.size());
	}

	std::vector<double> Calculations::weighted_sum(/*I*/ const std::vector<double>& values_a, 
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

	std::vector<double> Calculations::weighted_sum(/*I*/ const std::vector<std::vector<double>>& columns_of_values,
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

	std::vector<double> Calculations::rebalanced_weighted_sum(/*I*/ const std::vector<std::vector<double>>& returns,
								/*I*/ const std::vector<double>& weights,
								/*I*/ const size_t rebalancing_term){
		if(weights.empty() || returns.empty()){
			std::cout << "Calculations::rebalanced_weighted_sum: empty vectors" << std::endl;
		}

		size_t number_of_securities = returns.size();
		std::vector<double> results(returns[0].size(), 0);
		std::vector<std::vector<double>> compounded = {};

		for (int mm=0; mm<weights.size(); mm++){
			compounded[mm].emplace_back(weights[mm]);
		}

		for(int ii=0; ii<returns[0].size(); ii++){
			double rebalanced_total = 0;
			for (int jj=0; jj<returns.size(); jj++){
				compounded[jj].emplace_back(compounded[jj][ii] * (1+returns[jj][ii]/100.0));
				rebalanced_total += compounded[jj][ii+1];
			}
			if (ii%rebalancing_term==0){
				for (int ll=0; ll<returns.size(); ll ++){
					compounded[ll][ii+1] = rebalanced_total * weights[ll];
				}
			}
			results.emplace_back(rebalanced_total);
		}
		return results;
	}

	std::vector<double> Calculations::aggregate_returns_by_period(/*I*/ const std::vector<double>& trimmed_daily_returns,
									/*I*/ size_t intended_period){
		double current_value = 1.0;
		auto current_return_it = trimmed_daily_returns.begin();
		std::vector<double> aggregated ={};
		size_t aggregated_length = trimmed_daily_returns.size()/intended_period;
		aggregated.reserve(aggregated_length);

		bool is_end_reached = false;
		while(not is_end_reached){
			aggregated.emplace_back(current_value);
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

		return aggregated;
	}

	void Calculations::set_expected_return_strategy(/*I*/ ExpectedReturnStrategy* strategy){
		Calculations::expected_return_strategy = strategy;
	}

	double Calculations::get_expected_return(/*I*/ const std::vector<double>& returns){
		if (!expected_return_strategy){
			std::cout << "Calculations::get_expected_return: No strategy has been set.\n returning 0"
				<< std::endl;
			return 0;
		}
		return expected_return_strategy->get_expected_return(returns);
	}
}
