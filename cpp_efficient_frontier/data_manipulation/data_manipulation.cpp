#include "data_manipulation.hpp"

void test_data_manipulation(){
    std::cout<< "cpp_dat_manipulation" << std::endl;
}

namespace data{

data::Data::Data(std::ifstream& data_file_stream){
    std::cout << "file stream constructor" << std::endl;
}

}