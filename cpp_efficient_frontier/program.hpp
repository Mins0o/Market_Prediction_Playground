#pragma once

#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>

#include "data_manipulation/data_manipulation.hpp"
#include "calculations/calculations.hpp"

#include <chrono>
#include <iostream>

#define measureTime(name, func)                                 \
    do {                                                        \
        auto pre##name = std::chrono::steady_clock::now();      \
        func;                                                   \
        auto post##name = std::chrono::steady_clock::now();     \
        std::chrono::duration<double, std::milli> name##Time = post##name - pre##name; \
        std::cout << "time used in " #name ": " << name##Time.count() << " ms" << std::endl; \
    } while(0)

typedef struct{
	std::vector<double> security_returns;
	time_t start_date;
	time_t end_date;
	size_t index;
} security_column;

typedef struct {
	double sharpe_ratio;
	double expected_return;
	double risk;
	std::vector<double> weights;
} portfolio_data;

/*0*/
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
						security_data.get_end_date(index),
						index}));
		}
	}
}

/*0*/
void match_security_length(/*I*/ const data::Data& security_data,
			/*I*/ const std::vector<security_column>& selections,
			/*O*/ std::vector<security_column>& processed){
	time_t max_start_date = 0;
	size_t start_date_id = -1;
	time_t min_end_date = 999'999'999'999;
	size_t end_date_id = -1;

	for (auto security: selections){
		time_t start_date = security_data.get_start_date(security.index);
		time_t end_date = security_data.get_end_date(security.index);
		if (max_start_date < start_date){
			max_start_date = start_date;
			start_date_id = security.index;
		}
		if (min_end_date > end_date){
			min_end_date = end_date;
			end_date_id = security.index;
		}
	}
	std::cout << "using start date of: " << security_data.get_security_name(start_date_id)
		<< " " << ctime(&max_start_date);
	std::cout << "using end date of: " << security_data.get_security_name(end_date_id)
		<< " " << ctime(&min_end_date);

	processed = std::vector<security_column>{};
	for (auto security: selections){
		processed.emplace_back(security_column({
			security_data.trim(security.security_returns, max_start_date, min_end_date),
			max_start_date,
			min_end_date,
			security.index
		}));
	}
}

void compound_returns_to_values(/*I*/ const std::vector<security_column>& processed,
				/*O*/ std::vector<security_column>& columns_of_values){
	;
}

/*2*/
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

/*2*/
void mix_securities(/*I*/ const std::vector<security_column>& selections,
			/*I*/ const std::vector<double>& weights,
			/*O*/ std::vector<double>& mixed_returns){
	auto values = std::vector<std::vector<double>>();
	
	for (security_column col: selections){
		values.emplace_back(calculations::Calculations::value_series(col.security_returns));
	}
	
	auto mixed_values = calculations::Calculations::weighted_sum(values, weights);
	
	mixed_returns = calculations::Calculations::values_to_change(mixed_values);
}

/*2*/
portfolio_data get_portfolio_stats(std::vector<double> portfolio_returns, double risk_free_rate = 0){
	double exp_ret = calculations::Calculations::get_expected_return(portfolio_returns);
	double stdev = calculations::Calculations::standard_deviation(portfolio_returns);
	double sharpe_r = (exp_ret - risk_free_rate)/stdev;
	return portfolio_data({sharpe_r, exp_ret, stdev, {}});
}

/*1*/
void run_simulation(/*I*/ const std::vector<security_column>& selections,
			/*O*/ std::vector<portfolio_data>& sim_results,
			/*I*/ double risk_free_rate=0.01){
	std::vector<double> mixed_returns;
	size_t number_of_securities = selections.size();
	auto weights = make_random_weights(number_of_securities);
	
	mix_securities(selections, weights, mixed_returns);
	
	auto pf_data = get_portfolio_stats(mixed_returns, risk_free_rate);

	pf_data.weights = weights;
	sim_results.emplace_back(pf_data);
}

/*1*/
void get_optimal(/*I*/ const std::vector<portfolio_data>& sim_results,
		/*O*/ portfolio_data (&optimal_mixes)[3]){
	double max_sharpe = -999'999'999;
	double mxs_ret = 0;
	double mxs_risk = 0;
	double max_ret = -999'999'999;
	double mxr_sharpe = 0;
	double mxr_risk = 0;
	double min_risk = 999'999'999;
	double mnr_ret = 0;
	double mnr_sharpe = 0;

	std::vector<double> max_sharpe_w = {};
	std::vector<double> max_ret_w = {};
	std::vector<double> min_risk_w = {};

	for (portfolio_data sim: sim_results){
		if(max_sharpe < sim.sharpe_ratio){
			max_sharpe = sim.sharpe_ratio;
			mxs_ret = sim.expected_return;
			mxs_risk = sim.risk;
			max_sharpe_w = sim.weights;
			optimal_mixes[0] = sim;
		}
		if(max_ret < sim.expected_return){
			max_ret = sim.expected_return;
			mxr_sharpe = sim.sharpe_ratio;
			mxr_risk = sim.risk;
			max_ret_w = sim.weights;
			optimal_mixes[0] = sim;
		}
		if(min_risk > sim.risk){
			min_risk = sim.risk;
			mnr_ret = sim.expected_return;
			mnr_sharpe = sim.sharpe_ratio;
			min_risk_w = sim.weights;
			optimal_mixes[0] = sim;
		}
	}

	std::cout << "max_sharpe :" << max_sharpe 
		<< "| ret: " << mxs_ret
		<< "| risk: " << mxs_risk
		<< " |weights: ";
	for (auto w : max_sharpe_w){
		std::cout << " " << w;
	}
	std::cout << std::endl;

	std::cout << "max_return :" << max_ret  
		<< "| sharpe: " << mxr_sharpe
		<< "| risk: " << mxr_risk
		<< " |weights: ";
	for (auto w : max_ret_w){
		std::cout << " " << w;
	}
	std::cout << std::endl;

	std::cout << "min_risk :" << min_risk   
		<< "| ret: " << mnr_ret
		<< "| sharpe: " << mnr_sharpe
	<< " |weights: ";
	for (auto w : min_risk_w){
		std::cout << " " << w;
	}
	std::cout << std::endl; 
}

/*0*/
void optimize_portfolio(/*I*/ const std::vector<security_column>& selections,
			/*O*/ portfolio_data (&optimal_mixes)[3]){
	const size_t num_simulations = 99'999;
	std::vector<portfolio_data> sim_results = {};

	std::vector<security_column> columns_of_values;
	compound_returns_to_values(selections, columns_of_values);

	DiscountedMeanStrategy discnt_strat(2);
	calculations::Calculations::set_expected_return_strategy(&discnt_strat);
	measureTime(run_simulation,
	for(int sim_cnt=0; sim_cnt < num_simulations; sim_cnt++){
		run_simulation(selections, sim_results);

		std::cout << sim_cnt+1 << " / " << num_simulations << "\r";
	}
	);
	std::cout << std::endl;

	// Write data to TSV file
	std::ofstream outfile("simulation_results.tsv");
	if (!outfile) {
		std::cerr << "Error opening file for writing!" << std::endl;
		return ;
	}
	for (const auto& pf_data : sim_results) {
		outfile << pf_data.risk << '\t' << pf_data.expected_return << '\n';
	}
	
	get_optimal(sim_results, optimal_mixes);
}
