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
PortfolioData Analysis::MixRandomRebalanceOnce(/*I*/ const std::vector<std::vector<double>>& returns,
							/*I*/ const double daily_risk_free_rate,
							/*I*/ T rebalance_parameter) const{
	std::vector<double> mixed_values;
	std::vector<double> mixed_returns;
	size_t number_of_securities = returns.size();

	auto weights = MakeRandomWeights(number_of_securities);

	if constexpr (std::is_same_v<T, size_t> || std::is_same_v<T, std::vector<size_t>>){
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

OptimalSet Analysis::FindOptimalMix(const SimulationResult& simulation_result) const{
	OptimalSet optimal_set = {};
	double max_sharpe = 0;
	double max_return = 0;
	double min_risk = 0;
	for (const PortfolioData& simulation_point: simulation_result.simulation_points){
		if (simulation_point.sharpe_ratio > max_sharpe){
			max_sharpe = simulation_point.sharpe_ratio;
			optimal_set[0] = simulation_point;
		}
		if (simulation_point.expected_return > max_return){
			max_return = simulation_point.expected_return;
			optimal_set[1] = simulation_point;
		}
		if (simulation_point.risk < min_risk){
			min_risk = simulation_point.risk;
			optimal_set[2] = simulation_point;
		}
	}
	return optimal_set;
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
			const std::string found_name = security_data.GetSecurityNameByIndex(index);
            std::cout << "Choosing " << found_name << " for " << choice << std::endl;
            security_selections_.emplace_back(security_data.GetSecurityByIndex(index));
            security_choices_names_.emplace_back(found_name);
        }
    }
}

template <typename T>
void Analysis::SetRebalancingParameter(T rebalancing_parameter){
	std::cout << "Setting rebalancing parameter" << std::endl;
	if constexpr (std::is_same_v<T, size_t>){
		rebalance_type_ = RebalanceType::kConstantInterval;
		rebalance_interval_ = rebalancing_parameter;
	} else if constexpr (std::is_same_v<T, std::vector<size_t>){
		rebalance_type_ = RebalanceType::kPredefinedIndices;
		rebalance_indices_ = rebalancing_parameter;
	} else {
		std::cout << "Rebalancing parameter is not of type size_t or std::vector<size_t>" << std::endl
		<< "Setting to long-term hold" << std::enld;
		rebalance_type_ = RebalanceType::kConstantInterval;
		rebalance_interval_ = 0;
	}
} 

void Analysis::OptimizePortfolio(size_t simulation_count){
	std::cout << "Optimizing portfolio" << std::endl;
	std::vector<std::vector<double>> returns;
	for (const auto& security: security_selections_){
		returns.emplace_back(security.security_returns);
	}

	std::vector<PortfolioData> simulation_points;
	for (size_t ii = 0; ii < simulation_count; ii++){
		simulation_points.emplace_back(
			// Unfortunately, the ternary operator cannot be used with defferent return types.
			rebalance_type_ == RebalanceType::kConstantInterval ?
			MixRandomRebalanceOnce(
				returns,
				0.01,
				rebalance_interval_) :
			MixRandomRebalanceOnce(
				returns,
				0.01,
				rebalance_indices_));
	}
	std::string simulation_id=std::to_string(simulated_results_.size());
	simulated_results_.emplace_back(SimulationResult({simulation_id, simulation_points}));
}

};