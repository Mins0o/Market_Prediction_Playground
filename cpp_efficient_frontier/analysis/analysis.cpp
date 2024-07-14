#include <random>
#include <algorithm>

#include "analysis.h"
#include "../data_manipulation/data_manipulation.h"
#include "../calculations/calculations.h"
#include "../data_manipulation/date_line/date_line.h"

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

size_t Analysis::GetStartingIndex() const{
	return date_line_.MatchDateIndex(start_date_);
}

template<typename T>
PortfolioData Analysis::MixRandomRebalanceOnce(/*I*/ const std::vector<std::vector<double>>& returns,
							/*I*/ T rebalance_parameter,
							/*I*/ const double daily_risk_free_rate) const{
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
		mixed_values = calculations::Calculations::WeightedSumOfValuesFromReturns(returns, weights);
		mixed_returns = calculations::Calculations::CalculateReturnsFromValueSeries(mixed_values);
	}
	auto pf_data = GetPortfolioStats(mixed_returns, weights, daily_risk_free_rate);
	return pf_data;
}

OptimalSet Analysis::FindOptimalMix(const SimulationResult& simulation_result) const{
	OptimalSet optimal_set = {};
	double max_sharpe = 0;
	double max_return = 0;
	double min_risk = 999'999'999;
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

void Analysis::UpdateStartDate(const time_t new_date, const DateUpdateMode mode){
	if (mode == DateUpdateMode::kUpdateToEarliest){
		start_date_ = std::min(start_date_, new_date);
	} else {
		if (start_date_ == 999'999'999){
			start_date_ = new_date;
		}
		start_date_ = std::max(start_date_, new_date);
	}
}

// public
Analysis::Analysis(){
    std::cout << "Analysis object created" << std::endl;
}

Analysis::Analysis(const data::Data& dataset, const std::vector<std::string>& security_choices){
	date_line_ = dataset.GetDateLine();
    std::cout << "Analysis object created with security choices" << std::endl;
    ChooseSecurities(dataset, security_choices);
}

void Analysis::ChooseSecurities(const data::Data& security_data, 
                                const std::vector<std::string>& security_choices){
	date_line_ = security_data.GetDateLine();
    for (const auto& choice: security_choices){
        size_t index = security_data.FindIndexBySecurityName(choice);
        if (index != -1){
			const std::string found_name = security_data.GetSecurityNameByIndex(index);
            std::cout << "Choosing " << found_name << " for " << choice << std::endl;
            security_selections_.emplace_back(security_data.GetSecurityByIndex(index));
            security_choices_names_.emplace_back(found_name);
			UpdateStartDate(security_data.GetSecurityByIndex(index).start_date, DateUpdateMode::kUpdateToLatest);
		} else {
			std::cout << "Security " << choice << " not found" << std::endl;
        }
    }
}

void Analysis::ShowSecurityChoices() const{
	std::cout << "Chosen securities are: " << std::endl;
	for (const auto& security: security_selections_){
		std::cout << "name: " << security.security_name << "\t"
				<< "start date: " << std::string(ctime(&security.start_date)).substr(0, 24) << "\t"
				<< "end date: " << std::string(ctime(&security.end_date)).substr(0,24) << std::endl;
	}
	std::cout << "Using start date of :" << ctime(&start_date_) << std::endl;
}

template <typename T>
void Analysis::SetRebalancingParameter(T rebalancing_parameter){
	std::cout << "Setting rebalancing parameter" << std::endl;
	if constexpr (std::is_same_v<T, size_t>){
		rebalance_type_ = RebalanceType::kConstantInterval;
		rebalance_interval_ = rebalancing_parameter;
	} else if constexpr (std::is_same_v<T, std::vector<size_t>>){
		rebalance_type_ = RebalanceType::kPredefinedIndices;
		rebalance_indices_ = rebalancing_parameter;
	} else {
		std::cout << "Rebalancing parameter is not of type size_t or std::vector<size_t>" << std::endl
		<< "Setting to long-term hold" << std::endl;
		rebalance_type_ = RebalanceType::kConstantInterval;
		rebalance_interval_ = 0;
	}
} 

OptimalSet Analysis::OptimizePortfolio(size_t simulation_count){
	std::cout << "Optimizing portfolio" << std::endl;
	std::vector<std::vector<double>> returns;
	for (const auto& security: security_selections_){
		size_t start_index = GetStartingIndex();
		auto starting_point = security.security_returns.begin() + start_index;
		returns.emplace_back(std::vector<double>(starting_point, security.security_returns.end()));
	}

	std::vector<PortfolioData> simulation_points;
	for (size_t ii = 0; ii < simulation_count; ii++){
		simulation_points.emplace_back(
			// Unfortunately, the ternary operator cannot be used with defferent return types.
			rebalance_type_ == RebalanceType::kConstantInterval ?
			MixRandomRebalanceOnce(
				returns,
				rebalance_interval_,
				0.01) :
			MixRandomRebalanceOnce(
				returns,
				rebalance_indices_,
				0.01));
	}
	std::string simulation_id=std::to_string(simulated_results_.size());
	SimulationResult current_result({simulation_id, simulation_points});
	simulated_results_.emplace_back(current_result);

	optimal_mixes_.emplace_back(FindOptimalMix(current_result));

	return optimal_mixes_.back();
}

};