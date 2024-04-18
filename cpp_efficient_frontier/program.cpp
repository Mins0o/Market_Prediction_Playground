#include "calculations/calculations.hpp"
#include "data_manipulation/data_manipulation.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>

void read_file(const char* file_path, std::ifstream& read_file){
    std::cout<< "read_file " << file_path << std::endl;
    read_file = std::ifstream(file_path);
}

std::vector<double> test_suite(const size_t stock_number, data::Data& testing_data){
    std::string symbol_name = testing_data.get_symbol_name(stock_number);
    auto start_date = testing_data.get_start_date(stock_number);
    auto end_date = testing_data.get_end_date(stock_number);
    std::cout << stock_number << " " << symbol_name << 
                "\nstart_date: " << ctime(&start_date) <<
                "end_date: " << ctime(&end_date);
    std::cin.get();

    std::vector<double> symbol_values = testing_data.select_symbol(stock_number);

    auto trimmed = testing_data.trim(symbol_values, start_date, end_date);

    std::vector<double> net_returns1;
    double net_return = calculations::Calculations::net_return(trimmed);
    calculations::Calculations::value_series(trimmed, net_returns1);
    double average = calculations::Calculations::average(trimmed);
    double stdv = calculations::Calculations::standard_deviation(trimmed);

    std::cout << trimmed.front() << " "
                << trimmed.back() << " " 
                << trimmed.size() << std::endl
                << symbol_name << std::endl
                << net_return << std::endl
                << "avg " << average << std::endl
                << "stdv " << stdv << std::endl;

    for(int ii = 0; ii < trimmed.size()/1'000; ii++){
        std::cout << trimmed[ii] << " " << net_returns1[ii] << std::endl;
        std::cin.get();
    }
    return(symbol_values);
}

int main(int argc, char* argv[]){
    /*
    put the data into a easier-to-handle format (Parse)
     - cropping(?) data horizontally (columns) and vertically (dates)
     - browse stocks (read names and select)
      - read stock names
      - select stock columns by names
    */

    std::ifstream testing_stream;
    read_file(argv[1], testing_stream);

    data::Data testing_data = data::Data(testing_stream);

    std::srand(std::time(0));

    std::cout << testing_data.get_symbols_count() << std::endl;
    
    tm temporary_tm = {0};
    strptime("2024-03-01", "%Y-%m-%d", &temporary_tm);
    std::time_t start_time = mktime(&temporary_tm);
    strptime("2024-04-01", "%Y-%m-%d", &temporary_tm);
    std::time_t end_time = mktime(&temporary_tm);


    for(int ii=0; ii<30; ii++){
        int stock_number1 = std::rand() % testing_data.get_symbols_count();
        auto values1 = test_suite(stock_number1, testing_data);
        auto trimmed01 = testing_data.trim(values1, start_time,end_time);
        int stock_number2 = std::rand() % testing_data.get_symbols_count();
        auto values2 = test_suite(stock_number2, testing_data);
        auto trimmed02 = testing_data.trim(values2, start_time,end_time);
        auto summed = calculations::Calculations::weighted_sum(trimmed01,0.3,trimmed02,0.7);

        for (size_t ii=0; ii < summed.size(); ii++){
            std::cout << trimmed01[ii] << "\t" << trimmed02[ii] << "\t" << summed[ii] << std::endl;
        }
    }
}

