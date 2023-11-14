#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <ctime>

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
    row_count_ = 0;
    reading_file_ = std::ifstream(".." / directory / file_name);
}

void DataReader::GetStockNameList(Tokens& stock_name_list){
    if(row_count_++ != 0) return;

    std::string first_line;
    std::getline(reading_file_, first_line);
    ParseLine(first_line.substr(5), stock_name_list);
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
    // std::cout<<ctime(&time)<<std::endl;
    dates.push_back(time);


    Tokens::iterator token = parsed_tokens.begin();
    for(int ii=1;ii<column_count_;ii++){
        std::string token_content = *(token+ii);
        float value;
        if (token_content == ""){
            value = 0.0f;
        }
        else{
            value = std::stof(token_content);
        }
        return_table[ii-1].emplace_back(value);
    }
}

void DataReader::ReadData(Data& data){
    Tokens& stock_names = data.stock_names_;
    Dates& dates = data.dates_;
    ReturnTable& return_table = data.returns_;

    OpenFile();
    GetStockNameList(stock_names);

    column_count_ = stock_names.size();

    std::cout << "Number of columns: " << column_count_ << std::endl;

    return_table.reserve(column_count_);
    for(int ii=0; ii<column_count_;ii++){
        std::vector<float> column;
        column.reserve(9000);
        return_table.emplace_back(column);
    }

    std::string line;
    while(std::getline(reading_file_, line)){
        ParseDataLine(line, dates, return_table);
        row_count_++;
    }
    std::cout<< "Number of rows: " << data.dates_.size()<<std::endl;
}

std::vector<float> Data::GetStock(std::string stock_name){
    return std::vector<float>();
};

Dates Data::GetDate(std::string target_date){
    return Dates();
}

}