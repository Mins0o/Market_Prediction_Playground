#include "calculations/calculations.hpp"
#include "data_manipulation/data_manipulation.hpp"
#include "calculations/expected_returns/expected_returns.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>

#include "program.hpp"

int main(int argc, char* argv[]){

	std::ifstream reading_file;
	reading_file.open(argv[1]);
	data::Data security_data = data::Data(reading_file);
	
	std::vector<std::string> security_choices = {"삼성전자", "대양제지", "하이브", "대웅제약"};
	std::vector<security_column> selections;
	choose_securities(security_data, security_choices, selections);
	std::cin.get();

	std::vector<security_column> processed;
	match_security_length(security_data, selections, processed);

	portfolio_data optimal_mix;
	optimize_portfolio(processed, optimal_mix);
	
	
	// read file
	// choose securities
	// choose investment term
	// get weights for maximum sharpe ratio, with the returns and risks
		// with the securities
		// for n times
			// make random weight mix
			// compound by terms
			// get expected return
			// get risk
			// get sharpe ratio
			// make a data with sharpe, expected return, risk, and weight
			// put the calculation result in an array
	// sort the array (by sharpe, or by expected return, or by risk)
	// get the desired outcome
}
