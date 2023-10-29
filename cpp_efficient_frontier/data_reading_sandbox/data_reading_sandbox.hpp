#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <fstream>

namespace data_reading_sandbox {

typedef std::vector<std::vector<float>> ReturnTable;
typedef std::vector<std::string> Tokens;
typedef std::vector<std::time_t> Dates;

class Data{
private:
    int stock_count_ = 0;
    int time_point_count_ = 0;
public:
    Tokens stock_names_;
    Dates dates_;
    ReturnTable returns_;
private:
    ;
public:
    std::vector<float> GetStock(std::string stock_name);
    Dates GetDate(std::string target_date);
};

class DataReader{
private:
    int line_count_ = 0;
    std::ifstream reading_file_;
public:
private:
    void OpenFile();
    void ParseLine(const std::string& line, Tokens& parsed_tokens);
    void GetStockNameList(Tokens& stock_name_list);
    void ParseDataLine(const std::string& line, Dates& dates, ReturnTable& return_table);
public:
    void ReadData(Data& data);
};

}