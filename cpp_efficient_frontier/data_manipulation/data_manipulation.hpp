#include <fstream>
#include <iostream>

void test_data_manipulation();

namespace data{
class Data{
private:
    size_t row_count_ = 0;
    size_t column_count_ = 0;
public:
    Data(std::ifstream& data_file_stream);
};
}