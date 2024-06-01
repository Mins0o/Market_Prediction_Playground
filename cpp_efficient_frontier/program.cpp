#include "calculations/calculations.h"
#include "data_manipulation/data_manipulation.h"
#include "calculations/expected_returns/expected_returns.h"

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
	// std::cin.get();

	std::vector<security_column> processed;
	match_security_length(security_data, selections, processed);

	portfolio_data optimal_mixes[3] = {};
	optimize_portfolio(processed, optimal_mixes);
	
	/**
	 *TODO
	 * choose investment term
	 * compound by terms
	*/
}
