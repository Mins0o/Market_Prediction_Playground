#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "data_reading_sandbox.hpp"

namespace data_reading_sandbox{
void DataReader::ParseLine(const std::string& line, 
                            Tokens& parsed_tokens){
    std::stringstream stream_data(line);
    std::string token;
    while (std::getline(stream_data, token, '\t')){
        parsed_tokens.push_back(token);
    }
}

void DataReader::OpenFile(){
    std::filesystem::path directory("data_csv");
    std::filesystem::path file_name("stocks.tsv");
    line_count_ = 0;
    reading_file_ = std::ifstream(".." / directory / file_name);
}

void DataReader::GetStockNameList(Tokens& stock_name_list){
    if(line_count_++ != 0) return;

    std::string first_line;
    std::getline(reading_file_, first_line);
    ParseLine(first_line, stock_name_list);
}

void DataReader::ParseDataLine(const std::string& line, 
                                Dates& dates,
                                ReturnTable&return_table){
    Tokens parsed_tokens;
    ParseLine(line, parsed_tokens);
    std::string date = parsed_tokens[0];
    tm temporary_tm = {0};
    strptime(date.c_str(),"%Y-%m-%d", &temporary_tm);
    std::time_t time = mktime(&temporary_tm);
    dates.push_back(time);
}

void DataReader::ReadData(Data& data){
    Tokens& stock_names = data.stock_names_;
    Dates& dates = data.dates_;
    ReturnTable& return_table = data.returns_;

    OpenFile();
    GetStockNameList(stock_names);
    std::string line;
    while(std::getline(reading_file_, line)){
        ParseDataLine(line, dates, return_table);
    }
}

std::vector<float> Data::GetStock(std::string stock_name){
    return std::vector<float>();
};

Dates Data::GetDate(std::string target_date){
    return Dates();
}

}