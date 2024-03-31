#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include<algorithm>

#include "data_manipulation.hpp"

namespace data{
/**
 * Takes in the file stream and parses it
*/
Data::Data(std::ifstream& data_file_stream){
    std::cout << "file stream constructor" << std::endl;
    date_list_.reserve(9000);
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

void Data::process_data_row_(/*I*/ time_t date, 
                             /*I*/ std::vector<double> data_row,
                             /*I*/ std::vector<bool>& symbol_life_tracker){
    size_t column_index = 0;
    for(double data_value: data_row){
        if (data_value != 0){
            if(symbol_life_tracker[column_index] == false){
                symbol_life_tracker[column_index] = true;
                symbol_start_date_list_[column_index] = date;
            }
            else{
                symbol_end_date_list_[column_index] = date;
            }
        }
        return_table_[column_index++].emplace_back(data_value);
    }
}

size_t Data::match_date_(time_t target, std::vector<time_t>::iterator& match){
    match = std::lower_bound(date_list_.begin(), date_list_.end(), target);
    return (match - date_list_.begin());
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

    std::vector<bool> is_symbol_alive_list(column_count_, false);
    symbol_start_date_list_ = std::vector<time_t>(column_count_, 0);
    symbol_end_date_list_ = std::vector<time_t>(column_count_, 9'999'999);

    while(std::getline(parsing_stream, data_line)){
        std::vector<double> data_row;
        
        std::time_t date = parse_data_line_(data_line, data_row);
        date_list_.emplace_back(date);

        process_data_row_(date, data_row, is_symbol_alive_list);
    }

}

std::vector<double> Data::trim(std::vector<double> full_length_symbol, time_t start, time_t end){
    std::vector<time_t>::iterator temp_it;
    size_t start_index = match_date_(start, temp_it);
    size_t end_index = match_date_(end, temp_it);
    auto start_it = full_length_symbol.begin();
    return std::vector<double>(start_it + start_index, start_it + end_index + 1);
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

std::time_t Data::get_start_date(size_t symbol_index) const{
    return symbol_start_date_list_[symbol_index];
}

std::time_t Data::get_end_date(size_t symbol_index) const{
    return symbol_end_date_list_[symbol_index];
}

size_t Data::time_point_count() const{
    return date_list_.size();
}
}
