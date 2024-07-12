#include "analysis.h"

namespace analysis{
void Analysis::ChooseSecurities(const data::Data& security_data, const std::vector<std::string>& security_choices){
    for (const auto& choice: security_choices){
        size_t index = security_data.FindIndexBySecurityName(choice);
        if (index != -1){
            std::cout << "Choosing " << security_data.GetSecurityNameByIndex(index) << " for " << choice << std::endl;
            security_selections_.emplace_back(SecurityColumn({
                security_data.GetSecurityNameByIndex(index),
                security_data.GetSecurityDataByIndex(index),
                security_data.GetStartDateByIndex(index),
                security_data.GetEndDateByIndex(index)
            }));
            security_choices_names_.emplace_back(choice);
        }
    }
};