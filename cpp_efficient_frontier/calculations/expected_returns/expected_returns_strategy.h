#pragma once
#ifndef EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNSTRATEGY_H_
#define EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNSTRATEGY_H_

#include<vector>

class ExpectedReturnStrategy{
public:
	virtual double GetExpectedReturn(const std::vector<double>& returns_list) = 0;
};

#endif // EFFICIENTFRONTIER_CALCULATIONS_EXPECTEDRETURNSTRATEGY_H_