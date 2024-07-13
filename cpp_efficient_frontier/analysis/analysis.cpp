#include <random>
#include <algorithm>

#include "analysis.h"
#include "data_manipulation/data_manipulation.h"
#include "calculations/calculations.h"

namespace analysis{
//private
std::vector<double> Analysis::MakeRandomWeights(const size_t count) const{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(0, 1);

	// Generate 'n' random numbers
	std::vector<double> random_numbers;
	for (int ii = 0; ii < count - 1; ii++) {
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

PortfolioData Analysis::GetPortfolioStats(const std::vector<double>& portfolio_returns, 
                                            const std::vector<double>& weights, 
                                            const double risk_free_rate) const{
	double exp_ret = calculations::Calculations::GetExpectedReturn(portfolio_returns);
	double stdev = calculations::Calculations::StandardDeviation(portfolio_returns);
	double sharpe_r = (exp_ret - risk_free_rate)/stdev;
	return PortfolioData({sharpe_r, exp_ret, stdev, weights});
}

template<typename T>
PortfolioData Analysis::RandomRebalancedMix(/*I*/ const std::vector<std::vector<double>>& returns,
							/*I*/ const double daily_risk_free_rate,
							/*I*/ T rebalance_parameter){
	std::vector<double> mixed_values;
	std::vector<double> mixed_returns;
	size_t number_of_securities = returns.size();

	auto weights = MakeRandomWeights(number_of_securities);

	if constexper (std::is_same_v<T, size_t> || std::is_same_v<T, std::vector<size_t>>){
		mixed_values = calculations::Calculations::WeightedSumOfValuesWithRebalancing(returns, weights, rebalance_parameter);
		mixed_returns = calculations::Calculations::CalculateReturnsFromValueSeries(mixed_values);
	} else {
		std::cout << "Rebalancing parameter is not of type size_t or std::vector<size_t>" << std::endl
		<< "Simulating" ;
		mixed_values = calculations::Calculations::WeightedSumOfValuesFrom(returns, weights);
		mixed_returns = calculations::Calculations::CalculateReturnsFromValueSeries(mixed_values);
	}
	auto pf_data = GetPortfolioStats(mixed_returns, weights, daily_risk_free_rate);
	return pf_data;
}

// public
Analysis::Analysis(){
    std::cout << "Analysis object created" << std::endl;
}

Analysis::Analysis(const data::Data& dataset, const std::vector<std::string>& security_choices){
    std::cout << "Analysis object created with security choices" << std::endl;
    ChooseSecurities(dataset, security_choices);
}

void Analysis::ChooseSecurities(const data::Data& security_data, 
                                const std::vector<std::string>& security_choices){
    for (const auto& choice: security_choices){
        size_t index = security_data.FindIndexBySecurityName(choice);
        if (index != -1){
            std::cout << "Choosing " << security_data.GetSecurityNameByIndex(index) << " for " << choice << std::endl;
            security_selections_.emplace_back(SecurityColumn({
                security_data.GetSecurityNameByIndex(index),
                security_data.GetSecurityDataByIndex(index),
                security_data.GetStartDateByIndex(index),
                security_data.GetEndDateByIndex(index)
            }));
            security_choices_names_.emplace_back(choice);
        }
    }
}

};