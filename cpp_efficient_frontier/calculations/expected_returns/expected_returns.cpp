#include <numeric>
#include <vector>
#include "expected_returns.hpp"

double ArithmeticMeanStrategy::get_expected_return(/*I*/ const std::vector<double>& returns_list){
     if (returns_list.empty()){
          return 0.0;
     }
     return std::reduce(returns_list.begin(), returns_list.end())/returns_list.size();
}

double StatisticalStrategy::get_expected_return(const std::vector<double>& returns_list){
     if (returns_list.empty()){
          return 0.0;
     }
     return 0.0;
}
