#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include<algorithm>

#include "data_manipulation.hpp"

void test_data_manipulation(){
    std::cout<< "cpp_data_manipulation" << std::endl;
}

namespace data{
/**
 * Takes in the file stream and parses it
*/
Data::Data(std::ifstream& data_file_stream){
    std::cout << "file stream constructor" << std::endl;
    date_list_.reserve(9000);
    start_date_list_ = std::vector<std::time_t>(column_count_, 0);
    end_date_list_ = std::vector<std::time_t>(column_count_, 9'999'999'999);
    parse_(data_file_stream);
}

Tokens_t& Data::tokenize_line_(/*I*/ const std::string line, 
                               /*O*/ Tokens_t& tokens_return) const{
    std::stringstream line_stream(line);
    std::string token;
    while (std::getline(line_stream, token, '\t')){
        tokens_return.emplace_back(token);
    }
    return tokens_return;
}

Tokens_t& Data::parse_first_line_(/*I*/ const std::string first_line,
                                  /*O*/ Tokens_t& symbol_names) const{
    symbol_names = tokenize_line_(first_line, symbol_names);
    symbol_names.erase(symbol_names.begin());
    return symbol_names;
}

std::time_t Data::extract_date_(/*I*/ const char* date_string) const{
   tm temporary_tm = {0};
   strptime(date_string, "%Y-%m-%d", &temporary_tm);
   std:time_t time = mktime(&temporary_tm);

   return time;
}

double Data::stof_token_(/*I*/ std::string value_string) const{
    if (value_string == "" || value_string ==" "){
        return 0;
    }
    return std::stof(value_string);
}

std::time_t Data::parse_data_line_(/*I*/ const std::string line,
                                   /*O*/ std::vector<double>& data_values) const{
    Tokens_t tokens = {};
    tokens = tokenize_line_(line, tokens);

    std::time_t time = extract_date_(tokens.front().c_str());
    
    for(Tokens_t::iterator ii = tokens.begin()+1; ii != tokens.end(); ii++){
        data_values.emplace_back(stof_token_(*ii));
    }

    return time;
}

void Data::init_return_table_(/*I*/size_t column_count, /*I*/size_t row_count){
    return_table_.reserve(column_count);
    for(int ii=0; ii<column_count; ii++){
        return_table_.emplace_back();
	return_table_.back().reserve(row_count);
    }
}

void Data::process_data_row_(std::vector<double> data_row, std::vector<bool>& symbol_life_tracker){
    size_t column_number = 0;
    for(double data_value: data_row){
        return_table_[column_number++].emplace_back(data_value);
    }
}

void Data::parse_(std::ifstream& parsing_stream){
    parsing_stream.clear();
    parsing_stream.seekg(0);

    std::string first_line;
    std::getline(parsing_stream, first_line);

    symbol_names_ = parse_first_line_(first_line, symbol_names_);
    column_count_ = symbol_names_.size();

    init_return_table_(column_count_, 9000);

    std::string data_line;
    size_t line_number = 0;

    std::vector<bool> is_symbol_alive_list(column_count_,false);

    while(std::getline(parsing_stream, data_line)){
        std::vector<double> data_row;
        
        std::time_t date = parse_data_line_(data_line, data_row);
        date_list_.emplace_back(date);

        process_data_row_(data_row, is_symbol_alive_list);
    }
    std::cout<<"finished parsing"<<std::endl;
}

std::time_t Data::get_date(size_t date_index) const{
    return date_list_[date_index];
}

std::vector<double> Data::select_symbol(size_t symbol_index) const{
    return return_table_[symbol_index];
}

std::string Data::get_symbol_name(size_t symbol_index) const{
    return symbol_names_[symbol_index];
}

size_t Data::time_point_count() const{
    return date_list_.size();
}
}
