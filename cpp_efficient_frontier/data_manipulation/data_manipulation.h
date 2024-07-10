#pragma once
#ifndef EFFICIENTFRONTIER_DATAMANIPULATION_DATAMANIPULATION_H_
#define EFFICIENTFRONTIER_DATAMANIPULATION_DATAMANIPULATION_H_

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>

typedef std::vector<std::vector<double>> FloatArray;
typedef std::vector<std::string> Tokens_t;

namespace data{

class Data{
private: // fields
	size_t row_count_ = 0;
	size_t column_count_ = 0;
	std::vector<std::time_t> date_list_;
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
	Tokens_t& TokenizeLine(/*I*/ const std::string line,
				/*O*/ Tokens_t& tokens_return) const;
	/**
	 * Takes the first line (:string)) of the file to get the table headers
	 * which is the securities of the security.
	 * @param first_line <string> first line of the data table file
	 * @return tokenized and trimmed securities of securities.
	 */
	Tokens_t& ParseFirstLine(/*I*/ const std::string& first_line,
				/*O*/ Tokens_t& security_name) const;

	/**
	 * Extracts time_t date from the cstring in %Y-%m-%d format
	 * @param date_string date string in %Y-%m-%d . const char* type
	 */
	std::time_t ExtractDate(/*I*/ const char* date_string) const;

	/**
	 * takes in a token and returns double value
	 * if the token is a empty string, return 0
	 */
	double StofToken(/*I*/ std::string value_string) const;

	/**
	 * Takes in a data line
	 * - separates date (first) and data (the rest)
	 * - parse data into vector of doubles
	 * @param line the string of line to parse
	 * @param data_values the output vector of double to contain the result
	 * @return date of the corresponding line in time_t type
	 */
	std::time_t ParseDataLine(/*I*/ const std::string& line,
					/*O*/ std::vector<double>& data_values) const;

	/**
	 * reserves memory space for the return table data
	 * @param column_count
	 * @param row_count
	 */
	void InitReturnTable(/*I*/size_t column_count, /*I*/size_t row_count);

	/**
	 * 
	 */
	void ProcessDataRow(/*I*/time_t date, 
				/*I*/const std::vector<double>& data_row, 
				/*IO*/std::vector<bool>& security_life_tracker);

	size_t MatchDate(time_t target, std::vector<time_t>::const_iterator& match) const;

	/**
	 * Parses the whole data into this class instance.
	 * @param parsing_stream the std::ifstream of the file you are trying to read.
	 */
	void ParseStream(std::ifstream& parsing_stream);
public: // methods
	/**
	 * Constructor with std::ifstream.
	 * Parses data from the file stream and stores in to this instance.
	 * 
	 * @param data_file_stream currently being developed for `.tsv` format
	 */
	Data(std::ifstream& data_file_stream);

	/**
	 * takes in a full length vector of doubles with a intended start date and end date.
	 * returns a new vector that is trimmed to the date range
	 * @param full_length_security the "full length" means in terms of the Data object's date list
	 * @param start locks to the available date of the Data object, in case the market was not available at the date.
	 * @param end locks to the available date of the Data object, in case the market was not available at the date.
	 */
	std::vector<double> TrimSecurityByDate(/*I*/ const std::vector<double>& full_length_security, /*I*/ time_t start, /*I*/ time_t end) const;

	std::time_t GetDate(size_t date_index) const;
	size_t FindIndexBySecurityName(const std::string& security_name) const;
	std::vector<double> GetSecurityByIndex(size_t security_index) const;
	std::string GetSecurityNameByIndex(size_t security_index) const;
	std::time_t GetStartDate(size_t security_index) const;
	std::time_t GetEndDate(size_t security_index) const;
	size_t GetSecuritiesCount() const;
	size_t TimePointCount() const;

};
}
#endif // EFFICIENTFRONTIER_DATAMANIPULATION_DATAMANIPULATION_H_