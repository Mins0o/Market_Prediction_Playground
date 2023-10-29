#include "data_reading_sandbox/data_reading_sandbox.hpp"

int main(){

    auto hello = data_reading_sandbox::DataReader();
    auto data = data_reading_sandbox::Data();
    hello.ReadData(data);
}