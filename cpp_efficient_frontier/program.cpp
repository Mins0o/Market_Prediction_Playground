#include "calculations/calculations.h"
#include "data_manipulation/data_manipulation.h"
#include "data_manipulation/date_line/date_line.h"
#include "calculations/expected_returns/expected_returns.h"
#include "analysis/analysis.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include "program.hpp"

int wrapper(int argc, char* argv[]){
	std::ifstream reading_file;
	reading_file.open(argv[1]);
	MEASURE_TIME(parsing,
	data::Data security_data = data::Data(reading_file);
	);

	std::vector<std::string> security_choices = {"ACE중국본토CSI300", "하이브", "NAVER", "Kodex 한국대만IT프리미어", "KOSEF 인도 Nifty50", "ACE 미국S&P 500", "ACE 일본Nikkei255", "TIGER 글로벌리튬&2차전지SOLACTIVE합성)()", "현대차", "종근당"};

	DiscountedMeanStrategy discnt_strat(1);
	calculations::Calculations::SetExpectedReturnStrategy(&discnt_strat);

	analysis::Analysis test_analysis(security_data, security_choices);

	// test_analysis.ShowSecurityChoices();

	auto test_dateline = security_data.GetDateLine();
	auto start_date = test_analysis.GetStartDate();
	auto first_day_of_months = test_dateline.GetDatesOfMonthIndices(start_date, data::DateLine::ExtractDate("2024-07-31"),1);

	std::cout<< "start date " << std::string(ctime(&start_date)).substr(0,24) << std::endl;

	test_analysis.SetRebalancingParameter(first_day_of_months);
	MEASURE_TIME(optimize,
	test_analysis.OptimizePortfolio(999'999);
	);
	test_analysis.PrintOptimalMixes(-1);
	return 0;
}

int main(int argc, char* argv[]){
	MEASURE_TIME(totalTime,
	int ret_v = wrapper(argc, argv);
	);
	return ret_v;
}
