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
    /**
     * Converts line into separated tokens
     * @param line line to be tokenized
     * @param tokens_return empty Tokens object to contain the result
     * @return reference of the same Tokens object given as output parameter.\n\t Usage is not necessary.
     * @note type Tokens is std::vector<std::string>
    */
    Tokens& tokenize_line_(/*I*/ const std::string line,
                           /*O*/ Tokens& tokens_return) const;
    /**
     * Takes the first line (:string)) of the file to get the table headers
     * which is the symbols of the security.
     * @param first_line <string> first line of the data table file
     * @return tokenized and trimmed symbols of securities.
    */
    Tokens& parse_first_line_(/*I*/ const std::string first_line,
                              /*O*/ Tokens& symbol_name) const;

    std::time_t parse_data_line_(/*I*/ const std::string line,
                                 /*O*/ std::vector<double>& data_values) const;

    /**
     * reserves memory space for the return table data
     * @param column_count
     * @param row_count
    */
    void init_return_table_(column_count, row_count);

    /**
     * Parses the whole data into this class instance.
     * @param parsing_stream the std::ifstream of the file you are trying to read.
    */
    void parse_(std::ifstream& parsing_stream);
public:
    /**
     * Constructor with std::ifstream.
     * Parses data from the file stream and stores in to this instance.
     * 
     * @param data_file_stream currently being developed for `.tsv` format
    */
    Data(std::ifstream& data_file_stream);

    void trim();
    void select_date_range();
    void select_stock();
};
}
