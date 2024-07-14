#include "calculations/calculations.h"
#include "data_manipulation/data_manipulation.h"
#include "calculations/expected_returns/expected_returns.h"
#include "analysis/analysis.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include "program.hpp"

int main(int argc, char* argv[]){
	std::ifstream reading_file;
	reading_file.open(argv[1]);
	MEASURE_TIME(parsing,
	data::Data security_data = data::Data(reading_file);
	);

	std::vector<std::string> security_choices = {"ACE중국본토CSI300", "하이브", "NAVER", "Kodex 한국대만IT프리미어", "KOSEF 인도 Nifty50", "ACE 미국S&P 500", "ACE 일본Nikkei255", "TIGER 글로벌리튬&2차전지SOLACTIVE합성)()", "현대차", "종근당"};

	if (argc<3){
	std::vector<security_column> selections;
	choose_securities(security_data, security_choices, selections);

	std::vector<security_column> processed;
	match_security_length(security_data, selections, processed);

	// [0][] daily rebalanced
	// [1][] monthly rebalanced
	// [2][] long term
	// [][0] max sharpe ratio
	// [][1] max return
	// [][2] min risk
	portfolio_data optimal_mixes[3][3] = {};
	for (std::string security_name: security_choices){
		std::cout << security_name << " | ";
	}
	std::cout << std::endl;
	optimize_portfolio(processed, optimal_mixes);

	std::cout << "Newest on the top" << std::endl;
	// simulate_timelapse(processed, 1,50);

	std::cout << "Current Max Sharpe Ratio Portfolio for Long Term" << std::endl;
	std::cout << "Expected Return: " << optimal_mixes[2][0].expected_return << std::endl;
	std::cout << "Risk: " << optimal_mixes[2][0].risk << std::endl;
	std::cout << "Sharpe Ratio: " << optimal_mixes[2][0].sharpe_ratio << std::endl;
	std::cout << "Weights: ";
	for (double weight: optimal_mixes[2][0].weights){
		std::cout << std::fixed << std::setprecision(4) << weight << " | ";
	}
	std::cout << std::endl;
	for (std::string security_name: security_choices){
		std::cout << security_name << "\t";
	}
	}else {
		std::cout << "hello" <<std::endl;

		DiscountedMeanStrategy discnt_strat(1);
		calculations::Calculations::SetExpectedReturnStrategy(&discnt_strat);
		analysis::Analysis test_analysis(security_data, security_choices);
		test_analysis.ShowSecurityChoices();
		auto hello_opt = test_analysis.OptimizePortfolio(9999);

		for (auto opt: hello_opt){
			std::cout << "Expected Return: " << opt.expected_return << std::endl;
			std::cout << "Risk: " << opt.risk << std::endl;
			std::cout << "Sharpe Ratio: " << opt.sharpe_ratio << std::endl;
			std::cout << "Weights: ";
			for (double weight: opt.weights){
				std::cout << std::fixed << std::setprecision(4) << weight << " | ";
			}
			std::cout << std::endl;
		}
	}
}
