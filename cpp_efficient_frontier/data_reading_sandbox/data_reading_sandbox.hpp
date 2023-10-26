#pragma once

#include <string>
#include <vector>
#include <ctime>

namespace data_reading_sandbox {

class Data{
private:
    std::vector<std::string> stock_names_;
    std::vector<std::time_t> dates_;
    std::vector<std::vector<float>> returns_;
public:
    std::vector<float> GetStock(std::string stock_name);
    std::vector<float> GetDate(std::string target_date);
};

class DataReader{
private:
public:
    Data ReadData();
};

}