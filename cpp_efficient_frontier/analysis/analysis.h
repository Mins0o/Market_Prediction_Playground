#ifndef EFFICIENTFRONTIER_ANALYSIS_ANALYSIS_H_
#define EFFICIENTFRONTIER_ANALYSIS_ANALYSIS_H_
#include <vector>
#include <string>

#include "data_manipulation/data_manipulation.h"

namespace analysis{
    
using PortfolioData = struct {
    double sharpe_ratio;
    double expected_return;
    double risk;
    std::vector<double> weights;
};

using SimulationResult = struct {
    std::string simulation_id;
    std::vector<PortfolioData> simulation_points;
};

class Analysis{
private:
    std::vector<PortfolioData> optimal_mixes_[3][3] = {};
    std::vector<std::string> security_choices_names_;
    std::vector<SecurityColumn> security_selections_;
    std::vector<SimulationResult> simullated_results_;
public:
    int dummy;
private:
    std::vector<double> MakeRandomWeights(/*I*/ const size_t count) const;
    PortfolioData GetPortfolioStats(/*I*/ const std::vector<double>&portfolio_returns,
                                    /*I*/ const std::vector<double>& weights, 
                                    /*I*/ double risk_free_rate = 0) const;
    template<typename T>
    PortfolioData RandomRebalancedMix(/*I*/ const std::vector<std::vector<double>>& returns,
                                /*I*/ const double daily_risk_free_rate = 0.01,
                                /*I*/ T rebalancing_parameter);
public:
    Analysis();
    Analysis(/*I*/ const data::Data& dataset, /*I*/ const std::vector<std::string>& security_choices);
    void ChooseSecurities(/*I*/ const data::Data& security_data, 
                        /*I*/ const std::vector<std::string>& security_choices);
    void ConfigureAnalysis();
    void OptimizePortfolio(/*I*/ const std::vector<SecurityColumn>& processed);
    void SimulateTimelapse(/*I*/ const std::vector<SecurityColumn>& processed, 
                            /*I*/ const size_t start, 
                            /*I*/ const size_t end);
    void PrintOptimalMixes();
};

} // namespace analysis

#endif // EFFICIENTFRONTIER_ANALYSIS_ANALYSIS_H_