#include "calculations/calculations.hpp"
#include "data_manipulation/data_manipulation.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>

#define s(x) std::setw(x) <<

void read_file(const char* file_path, std::ifstream& read_file){
    std::cout<< "read_file " << file_path << std::endl;
    read_file = std::ifstream(file_path);
}

std::vector<double> test_suite(/*I*/ const size_t security_index, 
                                /*I*/ data::Data& testing_data, 
                                /*O*/ std::vector<double>& stripped_returns,
                                /*O*/ time_t& start_date,
                                /*O*/ time_t& end_date){
    std::string security_name = testing_data.get_security_name(security_index);
    start_date = testing_data.get_start_date(security_index);
    end_date = testing_data.get_end_date(security_index);
    std::cout << security_index << " " << security_name << 
                "\nstart_date: " << ctime(&start_date) <<
                "end_date: " << ctime(&end_date);
    std::cin.get();

    std::vector<double> security_returns = testing_data.select_security(security_index);

    stripped_returns = testing_data.trim(security_returns, start_date, end_date);

    double net_value = calculations::Calculations::net_value(stripped_returns);
    std::vector<double> net_values = calculations::Calculations::value_series(stripped_returns);
    double average = calculations::Calculations::average(stripped_returns);
    double stdv = calculations::Calculations::standard_deviation(stripped_returns);

    std::cout << "front back length: " << stripped_returns.front() << " "
                << stripped_returns.back() << " " 
                << stripped_returns.size() << std::endl
                << "name: " << security_name << std::endl
                << "net_value: " << net_value << std::endl
                << "avg " << average << std::endl
                << "stdv " << stdv << std::endl;

    for(int ii = 1; ii < stripped_returns.size()/1'000; ii++){
        std::cout << s(3) ii << s(10) stripped_returns[ii-1] << s(10) net_values[ii] << std::endl;
    }
    std::cin.get();
    return(security_returns);
}

int main(int argc, char* argv[]){
    /*
    put the data into a easier-to-handle format (Parse)
     - cropping(?) data horizontally (columns) and vertically (dates)
     - browse securities (read names and select)
      - read security names
      - select security columns by names
    */

    std::ifstream testing_stream;
    read_file(argv[1], testing_stream);

    data::Data testing_data = data::Data(testing_stream);

    std::srand(std::time(0));

    std::cout << testing_data.get_securities_count() << std::endl;


    for(int ii=0; ii<30; ii++){

        int security_index1 = std::rand() % testing_data.get_securities_count();
        std::vector<double> stripped01={};
        time_t start_date01={};
        time_t end_date01={};

        auto returns1 = test_suite(security_index1, testing_data, stripped01, start_date01, end_date01);

        int security_index2 = std::rand() % testing_data.get_securities_count();
        std::vector<double> stripped02={};
        time_t start_date02={};
        time_t end_date02={};

        auto returns2 = test_suite(security_index2, testing_data, stripped02, start_date02, end_date02);

        time_t start_date = start_date01>start_date02?start_date01:start_date02;
        time_t end_date = end_date01<end_date02?end_date01:end_date02;

        auto trimmed_returns01 = testing_data.trim(returns1, start_date,end_date);
        auto trimmed_returns02 = testing_data.trim(returns2, start_date,end_date);

        std::vector<double> trimmed_values01 = calculations::Calculations::value_series(trimmed_returns01);
        std::vector<double> trimmed_values02 = calculations::Calculations::value_series(trimmed_returns02);
 
        auto summed = calculations::Calculations::weighted_sum(trimmed_values01,0.3,trimmed_values02,0.7);
        auto portfolio_retruns = calculations::Calculations::values_to_change(summed);

        std::cout << "start date" << ctime(&start_date) 
                    << "end date" << ctime(&end_date) << std::endl;
        std::cout << s(10) "return01" << s(10) "return02" << s(10) "value01" 
                    << s(10) "value02" << s(15) "combined value" << s(20) "portfolio returns" << std::endl;
        for (size_t ii=1; ii < summed.size() && ii < 30; ii++){
            std::cout << s(10) trimmed_returns01[ii-1] << s(10) trimmed_returns02[ii-1]
                        << s(10) trimmed_values01[ii] << s(10) trimmed_values02[ii]
                        << s(15) summed[ii] << s(20) portfolio_retruns[ii] << std::endl;
        }

        double p_expected_return = calculations::Calculations::expected_return_avg(portfolio_retruns);
        double p_risk = calculations::Calculations::standard_deviation(portfolio_retruns);

        std::cout << s(20) "expected_return (avg)" << s(10) "risk" << std::endl
                    << s(20) p_expected_return << s(10) p_risk << std::endl;
    }
}
