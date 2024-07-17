#include "calculations/calculations.h"
#include "data_manipulation/data_manipulation.h"
#include "data_manipulation/date_line/date_line.h"
#include "calculations/expected_returns/expected_returns.h"
#include "analysis/analysis.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <array>
#include <vector>
#include <chrono>
#include <algorithm>

#define MEASURE_TIME(name, func)                                 \
        auto pre##name = std::chrono::steady_clock::now();      \
        func;                                                   \
        auto post##name = std::chrono::steady_clock::now();     \
        std::chrono::duration<double, std::milli> name##Time = post##name - pre##name; \
        std::cout << "time used in " #name ": " << name##Time.count() << " ms" << std::endl;

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
	// time_t initial_end_date = data::DateLine::ExtractDate("2024-07-31");
	auto end_dates = test_analysis.GetEndDates();
	time_t initial_end_date = *std::min_element(end_dates.begin(), end_dates.end());
	auto first_day_of_months = test_dateline.GetDatesOfMonthIndices(start_date, initial_end_date,1);

	std::cout<< "start date " << std::string(ctime(&start_date)).substr(0,24) << std::endl;
	std::cout<< "end date " << std::string(ctime(&initial_end_date)).substr(0,24) << std::endl;

	test_analysis.SetRebalancingParameter(first_day_of_months);
	MEASURE_TIME(optimize,
	test_analysis.OptimizePortfolio(999'999);
	);
	test_analysis.PrintOptimalMixes(-1);

	time_t end_date = initial_end_date;
	for (int ii=1; ii<=20; ii++){
		end_date = test_dateline.AddBusinessDays(end_date, -5);
		test_analysis.SetEndDate(end_date);
		test_analysis.OptimizePortfolio(999'999);
		test_analysis.PrintOptimalMixes(-1);
	}
	std::cout << test_analysis.GetOptimalMixes().size() << std::endl;
	std::vector<std::vector<double>> optimal_weights;
	for (auto optimal_mix : test_analysis.GetOptimalMixes()){
		size_t optimal_option = static_cast<size_t>(analysis::OptimalOption::kMaxSharpeRatio);
		optimal_weights.push_back(optimal_mix[optimal_option].weights);
	}
	std::vector<double> optimal_weights_mean = calculations::Calculations::EqualWeightedAverage(optimal_weights);
	for (size_t ii=0; ii<optimal_weights_mean.size(); ii++){
		std::cout << optimal_weights_mean[ii] << " " << security_choices[ii] << std::endl;
	}
	
	return(0);
}

int main(int argc, char* argv[]){
	MEASURE_TIME(totalTime,
	int ret_v = wrapper(argc, argv);
	);
	return ret_v;
}
