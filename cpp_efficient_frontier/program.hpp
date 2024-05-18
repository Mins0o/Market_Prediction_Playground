#pragma once

#include <fstream>
#include <vector>

typedef std::vector<double> security_column;

typedef struct {
	double sharpe_ratio;
	double expected_return;
	double risk;
	std::vector<double> weights;
} portfolio_data;

void choose_securities(/*I*/ data::Data security_data,
			/*O*/ std::vector<security_column> selections){
	security_data.search_security_by_name("삼성전자");
}

void optimize_portfolio(/*I*/ std::vector<security_column>,
			/*O*/ portfolio_data optimal_mix){
	;
}
