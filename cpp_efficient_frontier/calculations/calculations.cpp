#include "calculations.hpp"
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>

namespace calculations {

double Calculations::net_value(const std::vector<double>& returns){
    double net_value = 1.0;
    for(double return_value: returns){
        net_value *= 1 + (return_value/100);
    }
    return net_value;
}

std::vector<double> Calculations::value_series(/*I*/ const std::vector<double>& returns){
    double net_value = 1.0;
    std::vector<double> values={};
    values.reserve(returns.size());
    values.emplace_back(net_value);
    for(double return_value: returns){
        net_value *= 1 + (return_value/100);
        values.emplace_back(net_value);
    }
    return values;
}

std::vector<double> Calculations::values_to_change(/*I*/ const std::vector<double>& stripped_values){
    double prev = stripped_values[0];
    std::vector<double> change_rates = {};
    change_rates.reserve(stripped_values.size());
    for (double value: stripped_values){
        change_rates.emplace_back((value/prev - 1)*100);
        prev = value;
    }
    return change_rates;
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

double Calculations::expected_return(const std::vector<double>& returns){
    return 0;
}

double Calculations::expected_return_avg(const std::vector<double>& returns){
    // double sum = 0;
    // for (double return_value: returns){
    //     sum += return_value;
    // }
    // return sum/returns.size();
    return Calculations::average(returns);
}

double Calculations::expected_return_histogram(const std::vector<double>& returns, double bin_size){
    return 0.0;
}

std::vector<double> Calculations::weighted_sum(/*I*/ const std::vector<double>& returns_a, 
                                  /*I*/ const double weight_a,
                                  /*I*/ const std::vector<double>& returns_b,
                                  /*I*/ const double weight_b){
    std::vector<double> result = {};
    result.reserve(returns_a.size());

    if (returns_a.size() != returns_b.size()){
        return result;
    }

    for (int ii = 0; ii < returns_a.size(); ii++){
        result.emplace_back(returns_a[ii]*weight_a + returns_b[ii]*weight_b);
    }
    return result;
}

}