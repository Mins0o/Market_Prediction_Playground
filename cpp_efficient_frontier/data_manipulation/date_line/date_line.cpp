#include <iostream>
#include <ctime>
#include <vector>

#include "date_line.h"

namespace data{

// private
time_t DateLine::ConstructDate(size_t year, size_t month, size_t day) const{
    struct tm temp_tm = {0};
    temp_tm.tm_year = year;
    temp_tm.tm_mon = month;
    temp_tm.tm_mday = day;
    temp_tm.tm_hour = 0;
    temp_tm.tm_min = 0;
    temp_tm.tm_sec = 0;
    return mktime(&temp_tm);
}

// end private
// public
std::time_t DateLine::ExtractDate(const char* date_string){
    tm temporary_tm = {0};
    strptime(date_string, "%Y-%m-%d", &temporary_tm);
    std:time_t time = mktime(&temporary_tm);

    return time;
}

time_t DateLine::GetDateAtIndex(size_t index) const{
    return date_list_[index];
}

size_t DateLine::MatchDateIndex(time_t target) const{
    std::vector<time_t>::const_iterator match = std::lower_bound(date_list_.begin(), date_list_.end(), target);
    return (match - date_list_.begin());
}


std::vector<size_t> DateLine::GetDatesOfMonthIndices(time_t start, time_t end, size_t target_day) const{
    std::vector<size_t> result;
    size_t start_year = localtime(&start)->tm_year;
    size_t start_month = localtime(&start)->tm_mon;
    size_t end_year = localtime(&end)->tm_year;
    size_t end_month = localtime(&end)->tm_mon;
    size_t start_value = 12*start_year + start_month;
    size_t end_value = 12*end_year + end_month;

    size_t start_day = localtime(&start)->tm_mday;
    if (start_day > target_day){
            start_value++;
    }
    
    size_t end_day = localtime(&end)->tm_mday;
    if (end_day < target_day){
            end_value--;
    }

    for(size_t ii = start_value; ii <= end_value; ii++){
            time_t temp_time = ConstructDate(ii/12 + 1900, ii%12, target_day);
            size_t index = MatchDateIndex(temp_time);
            if (index < date_list_.size()){
                    result.emplace_back(index);
            }
    }

    return result;
}

size_t DateLine::CountBusinessDays(time_t start, time_t end) const{
    size_t start_index = MatchDateIndex(start);
    size_t end_index = MatchDateIndex(end);
    return end_index - start_index;
}

time_t DateLine::AddBusinessDays(time_t start, size_t days) const{
    size_t start_index = MatchDateIndex(start);
    size_t end_index = start_index + days;
    if (end_index >= date_list_.size()){
        std::cout << "Warning: DateLine::AddBusinessDays: end_index is out of range. Returning the last date in the list." << std::endl;
        return date_list_[date_list_.size()-1];
    }
    if (end_index < 0){
        std::cout << "Warning: DateLine::AddBusinessDays: end_index is out of range. Returning the first date in the list." << std::endl;
        return date_list_[0];
    }
    return date_list_[end_index];
}

void DateLine::AddDateToList(time_t date){
    date_list_.emplace_back(date);
}

void DateLine::AddDatesToList(const std::vector<time_t>& dates){
    date_list_.insert(date_list_.end(), dates.begin(), dates.end());
}

// end public
} // namespace data