#include <random>
#include <algorithm>

#include "analysis.h"
#include "../data_manipulation/data_manipulation.h"
#include "../calculations/calculations.h"
#include "../data_manipulation/date_line/date_line.h"

#include <iostream>
#include <iomanip>

namespace analysis{

//private
void Analysis::UpdateStartDate(const SecurityColumn& target_security, const DateUpdateMode mode){
	time_t new_date = target_security.start_date;
	if (mode == DateUpdateMode::kUpdateToEarliest){
		if (new_date < start_date_){
			start_date_ = new_date;
			security_name_of_start_date_ = target_security.security_name;
			start_index_ = date_line_.MatchDateIndex(start_date_);
		}
	} else {
		if (new_date > start_date_ || start_date_ == 999'999'999){
			start_date_ = new_date;
			security_name_of_start_date_ = target_security.security_name;
			start_index_ = date_line_.MatchDateIndex(start_date_);
		}
	}
}

std::vector<size_t> Analysis::TrimRebalanceIndices(const std::vector<size_t>& rebalance_indices) const{
	std::vector<size_t> trimmed_indices;
	for (const auto& index: rebalance_indices){
		if (index >= start_index_){
			trimmed_indices.emplace_back(index - start_index_);
		}
	}
	return trimmed_indices;
}

std::vector<double> Analysis::TrimReturns(const std::vector<double>& returns) const{
	return std::vector<double>(returns.begin() + start_index_, returns.end());
}

// ---
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
	} else {
		std::cout << "Rebalancing parameter is not of type size_t or std::vector<size_t>" << std::endl
		<< "Simulating longterm strategy" << std::endl;
		mixed_values = calculations::Calculations::WeightedSumOfValuesFromReturns(returns, weights);
	}
	mixed_returns = calculations::Calculations::CalculateReturnsFromValueSeries(mixed_values);
	auto pf_data = GetPortfolioStats(mixed_returns, weights, daily_risk_free_rate);
	return pf_data;
}

// ---
PortfolioData Analysis::GetPortfolioStats(const std::vector<double>& portfolio_returns, 
                                            const std::vector<double>& weights, 
                                            const double risk_free_rate) const{
	double exp_ret = calculations::Calculations::GetExpectedReturn(portfolio_returns);
	double stdev = calculations::Calculations::StandardDeviation(portfolio_returns);
	double sharpe_r = (exp_ret - risk_free_rate)/stdev;
	return PortfolioData({sharpe_r, exp_ret, stdev, weights});
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
// -- end private --

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
	size_t ii = 1;
    for (const auto& choice: security_choices){
        size_t index = security_data.FindIndexBySecurityName(choice);
        if (index != -1){
			const std::string found_name = security_data.GetSecurityNameByIndex(index);
            std::cout << std::setw(3) << ii++ ;
			std::cout << "  Choosing " << found_name << " for " << choice << std::endl;
            security_selections_.emplace_back(security_data.GetSecurityByIndex(index));
            security_choices_names_.emplace_back(found_name);
			UpdateStartDate(security_data.GetSecurityByIndex(index), DateUpdateMode::kUpdateToLatest);
		} else {
			std::cout << "Security " << choice << " not found" << std::endl;
        }
    }
}

// ---
void Analysis::ShowSecurityChoices() const{
	std::cout << "Chosen securities are: " << std::endl;
	for (const auto& security: security_selections_){
		std::cout << "name: " << std::setw(15) << security.security_name.substr(0,15) << "\t"
				<< "start date: " << std::string(ctime(&security.start_date)).substr(0, 24) << "\t"
				<< "end date: " << std::string(ctime(&security.end_date)).substr(0,24) << std::endl;
	}
	std::cout << "Using start date of :" << std::string(ctime(&start_date_)).substr(0,24) <<
			" from: " << security_name_of_start_date_ << std::endl;
}

void Analysis::PrintOptimalMixes(int index) const{
	std::array<PortfolioData,3> optimal_mixes;
	if (index < 0){
		optimal_mixes = optimal_mixes_.back();
	} else if (index < optimal_mixes_.size()){
		optimal_mixes = optimal_mixes_[index];
	}
	std::vector<std::string> mix_names = {"Max Sharpe Ratio", "Max Return", "Min Risk"};
	std::cout << std::endl;
	const bool kRedPrint[3][3] = {{false,false,true},{true,false,false},{false,true,false}};
	const std::string kGreen = "\033[32m";
	const std::string kReset = "\033[0m";
	for (int ii=0; ii<3; ii++){
		std::cout << mix_names[ii] << std::endl;
		auto optimal_mix = optimal_mixes[ii];
		std::cout << (kRedPrint[ii][0]?kGreen:"") << "Expected Return: " 
					<< (kRedPrint[ii][0]?kReset:"") << std::setprecision(4) << optimal_mix.expected_return << std::endl;
		std::cout << (kRedPrint[ii][1]?kGreen:"") << "Risk: " 
					<< (kRedPrint[ii][1]?kReset:"") << std::setprecision(4) << optimal_mix.risk << std::endl;
		std::cout << (kRedPrint[ii][2]?kGreen:"") << "Sharpe Ratio: " 
					<< (kRedPrint[ii][2]?kReset:"") << std::setprecision(4) << optimal_mix.sharpe_ratio << std::endl;
		std::cout << "     " ;
		for (int ii = 0; ii < security_choices_names_.size(); ii++){
			std::cout << std::setw(9) << ii+1;
		}
		std::cout << std::endl << "Weights: ";
		for (double weight: optimal_mix.weights){
			std::cout << std::fixed << std::setprecision(4) << weight << " | ";
		}
		std::cout << std::endl << std::endl;
	}
}

// ---
time_t Analysis::GetStartDate() const{
	return start_date_;
}

std::vector<OptimalSet> Analysis::GetOptimalMixes() const{
	return optimal_mixes_;
}

// ---

// ---
void Analysis::OptimizePortfolio(size_t simulation_count){
	std::cout << "Optimizing portfolio" << std::endl;
	std::vector<std::vector<double>> returns;
	for (const auto& security: security_selections_){
		returns.emplace_back(TrimReturns(security.security_returns));
	}

	auto rebalance_indices = TrimRebalanceIndices(rebalance_indices_);
	std::vector<PortfolioData> simulation_points;
	for (size_t ii = 0; ii < simulation_count; ii++){
		simulation_points.emplace_back(
			// Unfortunately, the ternary operator cannot be used with defferent return types.
			rebalance_type_ == RebalanceType::kConstantInterval ?
			MixRandomRebalanceOnce(
				returns, rebalance_interval_, 0.01) :
			MixRandomRebalanceOnce(
				returns, rebalance_indices, 0.01));
	}
	std::string simulation_id = std::to_string(simulated_results_.size());
	SimulationResult current_result({simulation_id, simulation_points});
	simulated_results_.emplace_back(current_result);

	auto optimal_set = FindOptimalMix(current_result);

	optimal_mixes_.emplace_back(optimal_set);
	std::cout << "Simulation id: " << simulation_id << "Simulation index" << optimal_mixes_.size()-1 << std::endl;
}

}; // namespace analysis