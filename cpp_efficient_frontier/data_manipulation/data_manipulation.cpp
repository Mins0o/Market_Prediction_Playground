#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <algorithm>
#include <chrono>

#include <rapidfuzz/fuzz.hpp>

#include "data_manipulation.h"
#include "date_line/date_line.h"

namespace data{
	/**
	 * Takes in the file stream and parses it
	 */

	template <typename Sentence1, typename Iterable, typename Sentence2 = typename Iterable::value_type>
	std::vector<std::pair<size_t, double>> extract(const Sentence1& query, const Iterable& choices, const double score_cutoff = 0.0)
	{
		std::vector<std::pair<size_t, double>> results;

		rapidfuzz::fuzz::CachedRatio<typename Sentence1::value_type> scorer(query);

		for (size_t ii=0; ii<choices.size(); ii++) {
			const auto& choice = choices[ii];
			double score = scorer.similarity(choice, score_cutoff);

			if (score >= score_cutoff) {
				results.emplace_back(ii, score);
			}
		}

		return results;
	}

	Data::Data(std::ifstream& data_file_stream){
		std::cout << "file stream constructor" << std::endl;
		ParseStream(data_file_stream);
	}

	Tokens_t& Data::TokenizeLine(/*I*/ const std::string line, 
					/*O*/ Tokens_t& tokens_return) const{
		std::stringstream line_stream(line);
		std::string token;
		while (std::getline(line_stream, token, '\t')){
			tokens_return.emplace_back(token);
		}
		return tokens_return;
	}

	Tokens_t& Data::ParseFirstLine(/*I*/ const std::string& first_line,
					/*O*/ Tokens_t& security_names) const{
		security_names = TokenizeLine(first_line, security_names);
		security_names.erase(security_names.begin());
		return security_names;
	}

	std::time_t Data::ExtractDate(/*I*/ const char* date_string) const{
		tm temporary_tm = {0};
		strptime(date_string, "%Y-%m-%d", &temporary_tm);
		std:time_t time = mktime(&temporary_tm);

		return time;
	}

	double Data::StofToken(/*I*/ std::string value_string) const{
		if (value_string == "" || value_string ==" "){
			return 0;
		}
		return std::stof(value_string);
	}

	std::time_t Data::ParseDataLine(/*I*/ const std::string& line,
					/*O*/ std::vector<double>& data_values) const{
		Tokens_t tokens = {};
		tokens = TokenizeLine(line, tokens);

		std::time_t time = ExtractDate(tokens.front().c_str());

		for(Tokens_t::iterator ii = tokens.begin()+1; ii != tokens.end(); ii++){
			data_values.emplace_back(StofToken(*ii));
		}

		return time;
	}

	void Data::InitReturnTable(/*I*/size_t column_count, /*I*/size_t row_count){
		return_table_.reserve(column_count);
		for(int ii=0; ii<column_count; ii++){
			return_table_.emplace_back();
			return_table_.back().reserve(row_count);
		}
	}

	void Data::ProcessDataRow(/*I*/ time_t date, 
				/*I*/ const std::vector<double>& data_row,
				/*I*/ std::vector<bool>& security_life_tracker){
		size_t column_index = 0;
		for(double data_value: data_row){
			if (data_value != 0){
				if(security_life_tracker[column_index] == false){
					security_life_tracker[column_index] = true;
					security_start_date_list_[column_index] = date;
				}
				else{
					security_end_date_list_[column_index] = date;
				}
			}
			return_table_[column_index++].emplace_back(data_value);
		}
	}

	void Data::ParseStream(std::ifstream& parsing_stream){
		parsing_stream.clear();
		parsing_stream.seekg(0);

		std::string first_line;
		std::getline(parsing_stream, first_line);

		security_names_ = ParseFirstLine(first_line, security_names_);
		column_count_ = security_names_.size();

		InitReturnTable(column_count_, 9000);

		std::string data_line;
		size_t line_number = 0;

		std::vector<bool> is_security_alive_list(column_count_, false);
		security_start_date_list_ = std::vector<time_t>(column_count_, 0);
		security_end_date_list_ = std::vector<time_t>(column_count_, 9'999'999);

		std::vector<time_t> date_list_;
		while(std::getline(parsing_stream, data_line)){
			std::vector<double> data_row;

			std::time_t date = ParseDataLine(data_line, data_row);
			date_list_.emplace_back(date);

			ProcessDataRow(date, data_row, is_security_alive_list);
		}
		date_line_.AddDatesToList(date_list_);
	}

	std::vector<double> Data::TrimSecurityByDate(const std::vector<double>& full_length_security, time_t start, time_t end) const{
		const size_t start_index = date_line_.MatchDateIndex(start);
		const size_t end_index = date_line_.MatchDateIndex(end);
		const size_t max_index = full_length_security.size();
		auto start_it = full_length_security.begin();
		return std::vector<double>(start_it + start_index, start_it + std::min(end_index + 1, max_index));
	}

	size_t Data::FindIndexBySecurityName(const std::string& security_name) const{
		auto results = extract(security_name, security_names_, 80);
		if (results.empty()){
			std::cout << "Search result for \"" << security_name << "\" was not found." << std::endl;
			std::cout << "Did you mean:"<<std::endl;
			for(const auto& candidate: extract(security_name, security_names_,49)){
				std::cout << this->GetSecurityNameByIndex(candidate.first) << std::endl;
			}
		}

		// get Max
		double max_score = 0;
		size_t match = -1;
		for (const auto& result : results){
			if (result.second > max_score){
				match = result.first;
				max_score = result.second;
			}
		}
		return match;
	}

	std::vector<double> Data::GetSecurityByIndex(size_t security_index) const{
		return return_table_[security_index];
	}

	std::string Data::GetSecurityNameByIndex(size_t security_index) const{
		return security_names_[security_index];
	}

	std::time_t Data::GetStartDate(size_t security_index) const{
		return security_start_date_list_[security_index];
	}

	std::time_t Data::GetEndDate(size_t security_index) const{
		return security_end_date_list_[security_index];
	}

	size_t Data::GetSecuritiesCount() const{
		return column_count_;
	}
}
