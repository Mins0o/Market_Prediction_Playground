#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>

void test_data_manipulation();

typedef std::map<std::time_t,size_t> DateMap;
typedef std::map<std::string,size_t> NameMap;
typedef std::vector<std::vector<double>> FloatArray;
typedef std::vector<std::string> Tokens;

namespace data{
class Data{
private:
    size_t row_count_ = 0;
    size_t column_count_ = 0;
    DateMap date_map_;
    NameMap name_map_;
    FloatArray return_table_;
    Tokens symbol_names_;
public:
    int dummy;
private:
    Tokens& tokenize_line_(/*I*/ const std::string line,
                           /*O*/ Tokens& tokens_return) const;
    /**
     * Takes the first line (:string)) of the file to get the table headers
     * which is the symbols of the security.
     * @param first_line <string> first line of the data table file
     * @return tokenized and trimmed symbols of securities.
    */
    Tokens& parse_first_line_(/*In*/ const std::string first_line,
                              /*Out*/ Tokens& tokens_return) const;
    void parse_(std::ifstream& parsing_stream);
public:
    /**
     * Constructor with std::ifstream.
     * Parses data from the file stream and stores in to this instance.
     * 
     * @param parsing_stream currently being developed for `.tsv` format
    */
    Data(std::ifstream& data_file_stream);

    void trim();
    void select_date_range();
    void select_stock();
};
}