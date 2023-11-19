#include "calculations.hpp"
#include "../data_reading_sandbox/data_reading_sandbox.hpp"


namespace calculations {

float Calculations::NetReturn(std::vector<float> stock_column){
    double net_value = 1.0;
    for(double value: stock_column){
        net_value *= 1 + (value/100);
    }
    return net_value;
}

;

}