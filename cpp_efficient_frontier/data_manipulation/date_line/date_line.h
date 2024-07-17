#ifndef EFFICIENT_DATAMANIPULATION_DATELINE_H_
#define EFFICIENT_DATAMANIPULATION_DATELINE_H_

#include <vector>
#include <ctime>

namespace data{
class DateLine {
private:
    std::vector<time_t> date_list_;
public:
    int dummy;
private:
    time_t ConstructDate(size_t year, size_t month, size_t day) const;
public:
    static std::time_t ExtractDate(/*I*/ const char* date_string);
    time_t GetDateAtIndex(size_t index) const;
    size_t MatchDateIndex(time_t target, bool verbose=false) const;
    std::vector<size_t> GetDatesOfMonthIndices(time_t start, time_t end, size_t day) const;
    size_t CountBusinessDays(time_t start, time_t end) const;
    time_t AddBusinessDays(time_t start, size_t days) const;
    void AddDateToList(time_t date);
    void AddDatesToList(const std::vector<time_t>& dates);
};

}

#endif // EFFICIENT_DATAMANIPULATION_DATELINE_H_