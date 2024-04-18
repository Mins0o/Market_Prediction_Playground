#include "calculations/calculations.hpp"
#include "data_manipulation/data_manipulation.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>

void read_file(const char* file_path, std::ifstream& read_file){
    std::cout<< "read_file " << file_path << std::endl;
    read_file = std::ifstream(file_path);
}

std::vector<double> test_suite(const size_t security_index, data::Data& testing_data){
    std::string security_name = testing_data.get_security_name(security_index);
    auto start_date = testing_data.get_start_date(security_index);
    auto end_date = testing_data.get_end_date(security_index);
    std::cout << security_index << " " << security_name << 
                "\nstart_date: " << ctime(&start_date) <<
                "end_date: " << ctime(&end_date);
    std::cin.get();

    std::vector<double> security_values = testing_data.select_security(security_index);

    auto trimmed = testing_data.trim(security_values, start_date, end_date);

    std::vector<double> net_returns1;
    double net_return = calculations::Calculations::net_return(trimmed);
    calculations::Calculations::value_series(trimmed, net_returns1);
    double average = calculations::Calculations::average(trimmed);
    double stdv = calculations::Calculations::standard_deviation(trimmed);

    std::cout << trimmed.front() << " "
                << trimmed.back() << " " 
                << trimmed.size() << std::endl
                << security_name << std::endl
                << net_return << std::endl
                << "avg " << average << std::endl
                << "stdv " << stdv << std::endl;

    for(int ii = 0; ii < trimmed.size()/1'000; ii++){
        std::cout << trimmed[ii] << " " << net_returns1[ii] << std::endl;
        std::cin.get();
    }
    return(security_values);
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
    
    tm temporary_tm = {0};
    strptime("2024-03-01", "%Y-%m-%d", &temporary_tm);
    std::time_t start_time = mktime(&temporary_tm);
    strptime("2024-04-01", "%Y-%m-%d", &temporary_tm);
    std::time_t end_time = mktime(&temporary_tm);


    for(int ii=0; ii<30; ii++){
        int security_index1 = std::rand() % testing_data.get_securities_count();
        auto values1 = test_suite(security_index1, testing_data);
        auto trimmed01 = testing_data.trim(values1, start_time,end_time);
        int security_index2 = std::rand() % testing_data.get_securities_count();
        auto values2 = test_suite(security_index2, testing_data);
        auto trimmed02 = testing_data.trim(values2, start_time,end_time);
        auto summed = calculations::Calculations::weighted_sum(trimmed01,0.3,trimmed02,0.7);

        for (size_t ii=0; ii < summed.size(); ii++){
            std::cout << trimmed01[ii] << "\t" << trimmed02[ii] << "\t" << summed[ii] << std::endl;
        }
    }
}

