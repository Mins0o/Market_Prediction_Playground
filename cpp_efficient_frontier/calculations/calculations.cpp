#include "calculations.hpp"

namespace calculations {

float Calculations::net_return(const std::vector<double>& returns){
    double net_value = 1.0;
    for(double return_value: returns){
        net_value *= 1 + (return_value/100);
    }
    return net_value;
}

void Calculations::get_acc_return_list(const std::vector<double>& returns, std::vector<double>& values){
    double net_value = 1.0;
    values.reserve(returns.size());
    values.emplace_back(net_value);
    for(double return_value: returns){
        net_value *= 1 + (return_value/100);
        values.emplace_back(net_value);
    }
}

}