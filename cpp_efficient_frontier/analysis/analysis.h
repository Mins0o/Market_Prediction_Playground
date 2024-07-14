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
    std::vector<OptimalSet> optimal_mixes_ = {};
    std::vector<std::string> security_choices_names_;
    std::vector<SecurityColumn> security_selections_;
    std::vector<SimulationResult> simulated_results_;
    RebalanceType rebalance_type_ = RebalanceType::kConstantInterval;
    size_t rebalance_interval_ = 0;
    std::vector<size_t> rebalance_indices_ = {};
    time_t start_date_ = 999'999'999;
    data::DateLine date_line_;
public:
    int dummy;
private:
    std::vector<double> MakeRandomWeights(/*I*/ const size_t count) const;
    PortfolioData GetPortfolioStats(/*I*/ const std::vector<double>&portfolio_returns,
                                    /*I*/ const std::vector<double>& weights, 
                                    /*I*/ double risk_free_rate = 0) const;
    size_t GetStartingIndex() const;
    template<typename T>
    PortfolioData MixRandomRebalanceOnce(/*I*/ const std::vector<std::vector<double>>& returns,
                                /*I*/ T rebalancing_parameter,
                                /*I*/ const double daily_risk_free_rate = 0.01) const;

    OptimalSet FindOptimalMix(/*I*/ const SimulationResult& simulation_result) const;
    void UpdateStartDate(/*I*/ time_t new_date, /*I*/ DateUpdateMode mode = DateUpdateMode::kUpdateToEarliest);
public:
    Analysis();
    Analysis(/*I*/ const data::Data& dataset, /*I*/ const std::vector<std::string>& security_choices);
    void ChooseSecurities(/*I*/ const data::Data& security_data, 
                        /*I*/ const std::vector<std::string>& security_choices);
    void ConfigureAnalysis();
    void ShowSecurityChoices() const;
    template <typename T>
    void SetRebalancingParameter(T rebalancing_parameter);
    OptimalSet OptimizePortfolio(size_t simulation_count = 9999);
    void SimulateTimelapse(/*I*/ const std::vector<SecurityColumn>& processed, 
                            /*I*/ const size_t start, 
                            /*I*/ const size_t end);
    void PrintOptimalMixes();
};

} // namespace analysis

#endif // EFFICIENTFRONTIER_ANALYSIS_ANALYSIS_H_