#include <iostream>
#include <fstream>
#include <sstream>

#include "data_manipulation.hpp"

void test_data_manipulation(){
    std::cout<< "cpp_dat_manipulation" << std::endl;
}

namespace data{

/**
 * Takes in the file stream and parses it
*/
data::Data::Data(std::ifstream& data_file_stream){
    std::cout << "file stream constructor" << std::endl;
    parse_(data_file_stream);
}

Tokens& Data::tokenize_line_(/*I*/ const std::string line, 
                             /*O*/ Tokens& tokens_return) const{
    std::stringstream line_stream(line);
    std::string token;
    while (std::getline(line_stream, token, '\t')){
        tokens_return.push_back(token);
    }
    return tokens_return;
}

Tokens& Data::parse_first_line_(/*I*/ const std::string first_line,
                                /*O*/ Tokens& symbol_names) const{
    symbol_names = tokenize_line_(first_line, symbol_names);
    symbol_names.erase(symbol_names.begin());
    return symbol_names;
}

void Data::parse_(std::ifstream& parsing_stream){
    parsing_stream.clear();
    parsing_stream.seekg(0);

    std::string first_line;

    std::getline(parsing_stream, first_line);
    symbol_names_ = parse_first_line_(first_line, symbol_names_);
    column_count_ = symbol_names_.size();

    return_table_.reserve(column_count_);
    for(int ii=0; ii<column_count_; ii++){
        return_table_.emplace_back();
        return_table_.back().reserve(9000);
    }
}

}