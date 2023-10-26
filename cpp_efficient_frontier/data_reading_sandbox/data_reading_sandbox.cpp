#include <iostream>
#include <fstream>
#include <filesystem>

#include "data_reading_sandbox.hpp"

using namespace data_reading_sandbox;

Data DataReader::ReadData(){
    std::filesystem::path directory("data_csv");
    std::filesystem::path file_name("stocks.tsv");
    std::ifstream reading_file(".." / directory / file_name);

    int line_count = 0;
    std::string read_text;
    while (getline(reading_file, read_text) && line_count++ <30){
        std::cout << read_text << std::endl;
    }
    return Data();
};

std::vector<float> Data::GetStock(std::string stock_name){
    return std::vector<float>();
};

std::vector<float> Data::GetDate(std::string target_date){
    return std::vector<float>();
}