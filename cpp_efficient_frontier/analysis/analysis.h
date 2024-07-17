#ifndef EFFICIENTFRONTIER_ANALYSIS_ANALYSIS_H_
#define EFFICIENTFRONTIER_ANALYSIS_ANALYSIS_H_
#include <vector>
#include <string>
#include <array>

#include "../data_manipulation/data_manipulation.h"
#include "../data_manipulation/date_line/date_line.h"

namespace analysis{
    
using PortfolioData = struct PortfolioData_t{
    double sharpe_ratio=-1;
    double expected_return=-1;
    double risk=999'999'999;
    std::vector<double> weights;
};

using SimulationResult = struct SimulationResult_t{
    std::string simulation_id;
    std::vector<PortfolioData> simulation_points;
};

/**
 * 0 max sharpe ratio
 * 1 max return
 * 2 min risk
 */
using OptimalSet = std::array<PortfolioData,3>;

enum class RebalanceType{
    kConstantInterval = 0,
    kPredefinedIndices
};

enum class DateUpdateMode{
    kUpdateToEarliest = 0,
    kUpdateToLatest
};

class Analysis{
private:
    std::vector<SecurityColumn> security_selections_;
    std::vector<std::string> security_choices_names_;
    data::DateLine date_line_;

    time_t start_date_ = 999'999'999;
    std::string security_name_of_start_date_ = "";
    size_t start_index_ = 0;
    time_t end_date_ = 99'999'999'999;
    size_t end_index_ = 99'999'999'999;

    RebalanceType rebalance_type_ = RebalanceType::kConstantInterval;
    size_t rebalance_interval_ = 0;
    std::vector<size_t> rebalance_indices_ = {};

    std::vector<SimulationResult> simulated_results_;
    std::vector<OptimalSet> optimal_mixes_ = {};

public:
    int dummy;

private:
    void UpdateStartDate(/*I*/ time_t start_date, std::string name);
    void UpdateStartDate(/*I*/ const SecurityColumn& target_security, 
                        /*I*/ DateUpdateMode mode = DateUpdateMode::kUpdateToEarliest);
    void UpdateEndDate(/*I*/ time_t end_date);
    std::vector<size_t> TrimRebalanceIndices(/*I*/ const std::vector<size_t>& rebalance_indices) const;
    std::vector<double> TrimReturns(/*I*/ const std::vector<double>&) const;

    std::vector<double> MakeRandomWeights(/*I*/ const size_t count) const;
    template<typename T> 
    PortfolioData MixRandomRebalanceOnce(/*I*/ const std::vector<std::vector<double>>& returns,
                                        /*I*/ T rebalancing_parameter,
                                        /*I*/ const double daily_risk_free_rate = 0.01) const;

    PortfolioData GetPortfolioStats(/*I*/ const std::vector<double>&portfolio_returns,
                                    /*I*/ const std::vector<double>& weights, 
                                    /*I*/ double risk_free_rate = 0) const;

    OptimalSet FindOptimalMix(/*I*/ const SimulationResult& simulation_result) const;
    
public:
    Analysis();
    Analysis(/*I*/ const data::Data& dataset, /*I*/ const std::vector<std::string>& security_choices);
    void ChooseSecurities(/*I*/ const data::Data& security_data, 
                        /*I*/ const std::vector<std::string>& security_choices);

    void ShowSecurityChoices() const;
    void PrintOptimalMixes(int index) const;

    time_t GetStartDate() const;
    time_t GetEndDate() const;
    std::vector<time_t> GetStartDates() const;
    std::vector<time_t> GetEndDates() const;
    std::vector<OptimalSet> GetOptimalMixes() const;

    void ConfigureAnalysis();
    void SetStartDate(time_t start_date);
    void SetEndDate(time_t end_date);

    template <typename RebalancingParam_t>
    void SetRebalancingParameter(RebalancingParam_t rebalancing_parameter);

    void OptimizePortfolio(size_t simulation_count = 399'999);
    void SimulateTimelapse(/*I*/ const std::vector<SecurityColumn>& processed, 
                            /*I*/ const size_t start, 
                            /*I*/ const size_t end);

};

template <typename RebalancingParam_t>
void Analysis::SetRebalancingParameter(RebalancingParam_t rebalancing_parameter){
    std::cout << "Setting rebalancing parameter" << std::endl;
    if constexpr (std::is_same_v<RebalancingParam_t, int>){
        rebalance_type_ = RebalanceType::kConstantInterval;
        rebalance_interval_ = rebalancing_parameter;
    } else if constexpr (std::is_same_v<RebalancingParam_t, std::vector<size_t>>){
        rebalance_type_ = RebalanceType::kPredefinedIndices;
        rebalance_indices_ = rebalancing_parameter;
    } else {
        std::cout << "Rebalancing parameter is not of type size_t or std::vector<size_t>" << std::endl
        << "Setting to long-term hold" << std::endl;
        rebalance_type_ = RebalanceType::kConstantInterval;
        rebalance_interval_ = 0;
    }
}

} // namespace analysis

#endif // EFFICIENTFRONTIER_ANALYSIS_ANALYSIS_H_