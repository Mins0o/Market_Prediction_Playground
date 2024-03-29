#include "data_reading_sandbox/data_reading_sandbox.hpp"
#include "calculations/calculations.hpp"
#include "data_manipulation/data_manipulation.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>

void read_file(const char* file_path, std::ifstream& read_file){
    std::cout<< "read_file " << file_path << std::endl;
    read_file = std::ifstream(file_path);
}

int main(int argc, char* argv[]){
    /*
    select data file
    read data file (as text I guess)
    
    ⬇ What should be passed into parsing?
    give ifstream for now.

    put the data into a easier-to-handle format (Parse)
     - get single stock record
      - get selected stock records
     - get date snapshot
      - get range of date snapsot
     - cropping(?) data horizontally (columns) and vertically (dates)
     - browse stocks (read names and select)
      - read stock names
      - select stock columns by names
    */

    std::ifstream testing_stream;
    read_file(argv[1], testing_stream);
   
    test_data_manipulation();

    data::Data testing_data = data::Data(testing_stream);


    std::srand(std::time(0));



    auto hello = data_reading_sandbox::DataReader();
    auto data = data_reading_sandbox::Data();

    hello.ReadData(data);
    auto hi = calculations::Calculations();

    data_reading_sandbox::StockColumn::dates = data.dates_;



    for(int ii=0; ii<30; ii++){
        int stock_lookup = std::rand() % 3526;
        std::string stock_name = testing_data.get_symbol_name(stock_lookup);
        std::cout << stock_lookup << " " << stock_name << " ";
        int hi;
        std::cin >> hi;
        data_reading_sandbox::StockColumn __stock_column;

        for(int jj=0; jj<testing_data.time_point_count(); jj++){
            double value = testing_data.select_symbol(stock_lookup)[jj];
            auto temp_t = testing_data.get_date(jj);
            std::cout << value << ctime(&temp_t) << std::endl;
        }


        std::vector<float> stock_lifetime = __stock_column.Strip();
        for(float value: stock_lifetime){
            std::cout << value << " ";
        }
        std::cout << std::endl;
        std::cout << "Net Value"
                << calculations::Calculations::NetReturn(stock_lifetime);
        std::cout << "Net of whole"
                << calculations::Calculations::NetReturn(__stock_column.returns_);
        std::cout << std::endl;

        double starting_value;
        std::cin >> starting_value;
    }
}

