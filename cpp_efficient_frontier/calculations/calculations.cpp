#include "calculations.hpp"
#include <vector>
#include <numeric>
#include <cmath>

namespace calculations {

double Calculations::net_return(const std::vector<double>& returns){
    double net_value = 1.0;
    for(double return_value: returns){
        net_value *= 1 + (return_value/100);
    }
    return net_value;
}

std::vector<double>& Calculations::value_series(const std::vector<double>& returns, std::vector<double>& values){
    double net_value = 1.0;
    values.reserve(returns.size());
    values.emplace_back(net_value);
    for(double return_value: returns){
        net_value *= 1 + (return_value/100);
        values.emplace_back(net_value);
    }
    return values;
}

double Calculations::average(const std::vector<double>& returns){
    if (returns.empty()){
        return 0;
    }
    return std::reduce(returns.begin(), returns.end())/returns.size();
}

double Calculations::standard_deviation(const std::vector<double>& returns){
    double average_return = Calculations::average(returns);
    double accumulate = 0;
    for(double return_v : returns){
        accumulate += (return_v-average_return) * (return_v-average_return);
    }
    return std::sqrt(accumulate / returns.size());
}

}