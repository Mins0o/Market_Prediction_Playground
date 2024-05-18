#pragma once

#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

typedef std::vector<double> security_column;

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
			selections.emplace_back(security_data.select_security(index));
		}
	}
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

void mix_securities(){
	std::cout << "mix_securities" << std::endl;
}

void get_portfolio_stats(){
	std::cout << "get_portfolio_stats" << std::endl;
}

void optimize_portfolio(/*I*/ std::vector<security_column> selections,
			/*O*/ portfolio_data& optimal_mix){
	size_t number_of_securities = selections.size();
	std::cout << number_of_securities;
	const size_t num_simulations = 999;
	
	for(int sim_cnt=0; sim_cnt < num_simulations; sim_cnt++){
		auto weights = make_random_weights(number_of_securities);
		for(const auto& weight:weights){
			std::cout << weight << " ";
		}
		std::cout << std::endl;
		// mix_securities();
		// get_portfolio_stats();
	}
}
