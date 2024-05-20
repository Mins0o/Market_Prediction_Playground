#pragma once

#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>

#include "data_manipulation/data_manipulation.hpp"
#include "calculations/calculations.hpp"

typedef struct{
	std::vector<double> security_returns;
	time_t start_date;
	time_t end_date;
} security_column;

typedef struct {
	double sharpe_ratio;
	double expected_return;
	double risk;
	std::vector<double> weights;
} portfolio_data;

void choose_securities(/*I*/ data::Data security_data,
			/*I*/ const std::vector<std::string>& security_choices,
			/*O*/ std::vector<security_column>& selections){
	for(const auto& choice: security_choices){
		size_t index = security_data.search_security_by_name(choice);
		if (index != -1){
			std::cout << "Choosing " << security_data.get_security_name(index)
				<< " for " << choice << std::endl;
			selections.emplace_back(security_column({security_data.select_security(index),
						security_data.get_start_date(index),
						security_data.get_end_date(index)}));
		}
	}
}

void match_security_length(/*I*/ const std::vector<security_column>& selections,
			/*O*/ std::vector<security_column>& processed){
	;
}

void compound_returns_to_values(/*I*/ const std::vector<security_column>& processed,
				/*O*/ std::vector<security_column>& columns_of_values){
	;
}

std::vector<double> make_random_weights(size_t number_of_securities){
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(0, 1);

	// Generate 'n' random numbers
	std::vector<double> random_numbers;
	for (int ii = 0; ii < number_of_securities - 1; ii++) {
		random_numbers.push_back(dis(gen));
	}

	// Sort the random numbers
	std::sort(random_numbers.begin(), random_numbers.end());

	// Calculate segment sizes
	std::vector<double> segments;
	double sum = 0.0;
	for (size_t ii = 0; ii < random_numbers.size(); ii++) {
		double segment_size = random_numbers[ii] - sum;
		segments.push_back(segment_size);
		sum += segment_size;
	}
	segments.push_back(1.0 - sum); // The last segment size

	return segments;
}

void mix_securities(/*I*/ const std::vector<security_column>& selections,
			/*I*/ const std::vector<double>& weights,
			/*O*/ std::vector<double>& mixed){
	auto selected_returns = std::vector<std::vector<double>>();
	for (security_column col: selections){
		selected_returns.emplace_back(col.security_returns);
	}
	mixed = calculations::Calculations::weighted_sum(selected_returns, weights);
}

void get_portfolio_stats(){
	// std::cout << "get_portfolio_stats" << std::endl;
}

void optimize_portfolio(/*I*/ const std::vector<security_column>& selections,
			/*O*/ portfolio_data& optimal_mix){
	size_t number_of_securities = selections.size();
	const size_t num_simulations = 999;

	std::vector<security_column> columns_of_values;
	compound_returns_to_values(selections, columns_of_values);

	for(int sim_cnt=0; sim_cnt < num_simulations; sim_cnt++){
		std::vector<double> mixed;	
		auto weights = make_random_weights(number_of_securities);
		mix_securities(selections, weights, mixed);
		std::cout << "w0: " << weights[0] << " w1: " << weights[1] << std::endl;
		for(int ii=3000;ii<selections[0].security_returns.size()/200+3000;ii++){
			std::cout << "s0: " << selections[0].security_returns[ii] 
				<< " s1: " << selections[1].security_returns[ii] 
				<< " m: " << mixed[ii] << std::endl;
		}
		std::cin.get();
	}
}
