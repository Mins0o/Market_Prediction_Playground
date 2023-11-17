#include "data_reading_sandbox/data_reading_sandbox.hpp"
#include "calculations/calculations.hpp"

#include <algorithm>

int main(){

    auto hello = data_reading_sandbox::DataReader();
    auto data = data_reading_sandbox::Data();
    hello.ReadData(data);
    auto hi = calculations::Calculations();
    calculations::Calculations::data_test(data);
    hi.data_test(data);

    for(int ii=0; ii<30; ii++){
        int stock_lookup = std::rand() % 3526;
        std::string stock_name = data.stock_names_[stock_lookup];
        std::cout << stock_lookup << " " << stock_name << " ";
        data.GetStock(stock_name);
    }
}