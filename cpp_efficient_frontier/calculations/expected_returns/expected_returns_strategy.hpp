#pragma once

#include<vector>

class ExpectedReturnStrategy{
public:
    virtual double get_expected_return(std::vector<double> returns_list) = 0;
};