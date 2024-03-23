#include "calculations.hpp"
#include "../data_reading_sandbox/data_reading_sandbox.hpp"


namespace calculations {

float Calculations::NetReturn(const std::vector<float>& returns){
    double net_value = 1.0;
    for(double return_value: returns){
        net_value *= 1 + (return_value/100);
    }
    return net_value;
}

void Calculations::Returns2Values(const std::vector<float>& returns, std::vector<float>& values){
    double net_value = 1.0;
    values.reserve(returns.size());
    values.emplace_back(net_value);
    for(double return_value: returns){
        net_value *= 1 + (return_value/100);
        values.emplace_back(net_value);
    }
}

}