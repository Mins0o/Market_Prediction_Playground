#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <ctime>
#include <algorithm>

#include "data_reading_sandbox.hpp"

namespace data_reading_sandbox{
void DataReader::OpenFile(){
    std::filesystem::path directory("data_csv");
    std::filesystem::path file_name("stocks.tsv");
    row_count_ = 0;
    std::cout << ".." / directory / file_name;
    reading_file_ = std::ifstream(".." / directory / file_name);
}

const void DataReader::ParseLine(const std::string& line, 
                            Tokens& parsed_tokens){
    std::stringstream stream_data(line);
    std::string token;
    while (std::getline(stream_data, token, '\t')){
        parsed_tokens.push_back(token);
    }
}

void DataReader::GetStockNameList(Tokens& stock_name_list){
    if(row_count_++ != 0) {
        std::cout << "GetStockNameList() called on non-first line" << std::endl;
        return;
    }

    std::string first_line;
    std::getline(reading_file_, first_line);
    // skipping the first column title "date"
    ParseLine(first_line.substr(5), stock_name_list);

    column_count_ = stock_name_list.size();
}

const void DataReader::ParseDataLine(const std::string& line, 
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
    for(int ii=1; ii<column_count_; ii++){
        std::string token_content = *(token + ii);
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
    data.stock_count = column_count_;

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
    data.time_point_count = row_count_ - 1;
    std::cout<< "Number of rows: " << data.dates_.size()<<std::endl;
}

void Data::GetStock(std::string target_name, StockColumn& stock_column){
    auto name_match = std::find(stock_names_.begin(), stock_names_.end(), target_name);

    if (name_match == stock_names_.end()){
        std::cout << target_name << "does not exist." << std::endl;
        return;
    }
    uint match_index = name_match - stock_names_.begin();
    stock_column = StockColumn(returns_[match_index]);
};

Dates Data::GetDate(std::string target_date){
    return Dates();
}

Dates StockColumn::dates;

StockColumn::StockColumn(std::vector<float> data){
    returns_ = data;
    get_valid_date_range();
}

StockColumn::StockColumn(){
    ;
}

void StockColumn::get_valid_date_range(){
    uint index = 0;
    bool started = false;

    uint end_index_candidate = 0;
    end_index_ = dates.size() - 1;
    bool zero_started = false;

    for (auto value_it = returns_.begin(); 
                value_it != returns_.end(); 
                        index++, value_it++){
        float current_value = *value_it;

        if(!started && (current_value != 0.0f)){
            started = true;
            start_index_ = index;
        }

        if(started && !zero_started && (current_value == 0.0f)){
            zero_started = true;
            end_index_candidate = index;
        }
        if(zero_started && (current_value != 0.0f)){
            zero_started = false;
        }
    }

    start_date_ = dates[start_index_];
    if(zero_started == true){
        end_index_ = end_index_candidate;
    }
    end_date_ = dates[end_index_];
}

std::vector<float> StockColumn::Strip(){
    uint required_size = end_index_ - start_index_;
    std::vector<float> stripped;
    stripped.reserve(required_size);
    for(int ii=start_index_; ii <= end_index_; ii++){
        stripped.emplace_back(returns_[ii]);
    }
    return stripped;
}

}