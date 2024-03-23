#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <fstream>

namespace data_reading_sandbox {

typedef std::vector<std::vector<float>> ReturnTable;
typedef std::vector<std::string> Tokens;
typedef std::vector<std::time_t> Dates;

class Data;
class StockColumn;

class DataReader{
private:
    size_t row_count_ = 0;
    size_t column_count_ = 0;
    std::ifstream reading_file_;
public:
private:
    void OpenFile();
    const void ParseLine(const std::string& line, Tokens& parsed_tokens);
    void GetStockNameList(Tokens& stock_name_list);
    const void ParseDataLine(const std::string& line, Dates& dates, ReturnTable& return_table);
public:
    void ReadData(Data& data);
};

class Data{
private:
public:
    uint stock_count = 0;
    uint time_point_count = 0;
    Tokens stock_names_;
    Dates dates_;
    ReturnTable returns_;
private:
    ;
public:
    void GetStock(std::string stock_name, StockColumn& stock_column);
    Dates GetDate(std::string target_date);
};

class StockColumn{
private:

public:
    static Dates dates;
    std::vector<float> returns_;
    std::time_t start_date_;
    std::time_t end_date_;
    uint start_index_;
    uint end_index_;
private:
    void get_valid_date_range();
public:
    StockColumn();
    StockColumn(std::vector<float> data);
    std::vector<float> Strip();
};

}