#include "data_reading_sandbox/data_reading_sandbox.hpp"
#include "calculations/calculations.hpp"

int main(){

    auto hello = data_reading_sandbox::DataReader();
    auto data = data_reading_sandbox::Data();
    hello.ReadData(data);
    auto hi = calculations::Calculations();
    calculations::Calculations::data_test(data);
    hi.data_test(data);
}