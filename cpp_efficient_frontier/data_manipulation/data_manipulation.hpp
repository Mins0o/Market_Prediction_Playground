#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>

#include <chrono>
#define MILLISECOND_NOW std::chrono::high_resolution_clock::now
#define COUNT_MILLISECONDS(x) ((std::chrono::duration<double, std::milli>)(x)).count()

typedef std::map<std::string,size_t> NameMap;
typedef std::vector<std::vector<double>> FloatArray;
typedef std::vector<std::string> Tokens_t;

namespace data{
class Data{
private: // fields
    size_t row_count_ = 0;
    size_t column_count_ = 0;
    std::vector<std::time_t> date_list_;
    NameMap name_map_;
    FloatArray return_table_;
    Tokens_t security_names_;
    std::vector<std::time_t> security_start_date_list_;
    std::vector<std::time_t> security_end_date_list_;
public: // fields
    int dummy;
private: // methods
    /**
     * Converts line into separated tokens
     * @param line line to be tokenized
     * @param tokens_return empty Tokens_t object to contain the result
     * @return reference of the same Tokens_t object given as output parameter.\n\t Usage is not necessary.
     * @note type Tokens_t is std::vector<std::string>
    */
    Tokens_t& tokenize_line_(/*I*/ const std::string line,
                             /*O*/ Tokens_t& tokens_return) const;
    /**
     * Takes the first line (:string)) of the file to get the table headers
     * which is the securities of the security.
     * @param first_line <string> first line of the data table file
     * @return tokenized and trimmed securities of securities.
    */
    Tokens_t& parse_first_line_(/*I*/ const std::string& first_line,
                                /*O*/ Tokens_t& security_name) const;

    /**
     * Extracts time_t date from the cstring in %Y-%m-%d format
     * @param date_string date string in %Y-%m-%d . const char* type
    */
    std::time_t extract_date_(/*I*/ const char* date_string) const;

    /**
     * takes in a potentially empty token and returns double value
    */
    double stof_token_(/*I*/ std::string value_string) const;

    /**
     * Takes in a data line
     * - separates date (first) and data (the rest)
     * - parse data into vector of doubles
     * @param line the string of line to parse
     * @param data_values the output vector of double to contain the result
     * @return date of the corresponding line in time_t type
    */
    std::time_t parse_data_line_(/*I*/ const std::string& line,
                                 /*O*/ std::vector<double>& data_values) const;

    /**
     * reserves memory space for the return table data
     * @param column_count
     * @param row_count
    */
    void init_return_table_(/*I*/size_t column_count, /*I*/size_t row_count);

    /**
     * 
    */
    void process_data_row_(/*I*/time_t date, 
                            /*I*/const std::vector<double>& data_row, 
                            /*IO*/std::vector<bool>& security_life_tracker);

    size_t match_date_(time_t target, std::vector<time_t>::iterator& match);

    /**
     * Parses the whole data into this class instance.
     * @param parsing_stream the std::ifstream of the file you are trying to read.
    */
    void parse_(std::ifstream& parsing_stream);
public: // methods
    /**
     * Constructor with std::ifstream.
     * Parses data from the file stream and stores in to this instance.
     * 
     * @param data_file_stream currently being developed for `.tsv` format
    */
    Data(std::ifstream& data_file_stream);

    std::vector<double> trim(const std::vector<double>& full_length_security, time_t start, time_t end);
    std::time_t get_date(size_t date_index) const;
    size_t search_security_by_name(const std::string& security_name) const;
    std::vector<double> select_security(size_t security_index) const;
    std::string get_security_name(size_t security_index) const;
    std::time_t get_start_date(size_t security_index) const;
    std::time_t get_end_date(size_t security_index) const;
    size_t get_securities_count() const;
    size_t time_point_count() const;
};
}
