#pragma once

#include <iostream>
#include <vector>

namespace calculations{

class Calculations{
public: 
    /**
     * calculates the end value assuming value of 1 at the start of the vector
     * @param returns vector of periodic returns on the security
    */
    static double net_return(/*I*/ const std::vector<double>& returns);

    /**
     * creates a vector that has compiles the periodic return into the values at the given time
     * @param returns vector of periodic returns on the security
    */
    static std::vector<double>& value_series(/*I*/ const std::vector<double>& returns, 
                                             /*O*/ std::vector<double>& values);

    /**
     * calulates the average of the periodic returns
     * @param returns vector of periodic returns on the security
    */
    static double average(/*I*/ const std::vector<double>& returns);

    /**
     * calculates the standard deviation of the periodic returns
     * @param returns vector of periodic returns on the security
    */
    static double standard_deviation(/*I*/ const std::vector<double>& returns);

    static double expected_return(const std::vector<double>& returns);

    /**
     * returns a vector that combines the two given vectors with the given weights
     * the weights do not need to sum up to 1
     * the length of the two vectors should match
     * @param returns_x 
     * @param weight_x 
    */
    static std::vector<double> weighted_sum(/*I*/ const std::vector<double>& returns_a, 
                                             /*I*/ const double weight_a,
                                             /*I*/ const std::vector<double>& returns_b,
                                             /*I*/ const double weight_b);
};

}
