#ifndef USERACTIVITYREPORT_HPP
#define USERACTIVITYREPORT_HPP

#include "IReport.hpp"
#include <string>

class UserActivityReport : public IReport
{
public:
    // supply repository file paths used to compute user activity
    UserActivityReport();

    void generate(const std::string& monthYear) override;

private:
    std::string reservationsPath;
    std::string passengersPath;
    std::string agentsPath;
    std::string adminsPath;

    static std::string extractMonthYearFromDateString(const std::string& dateStr);
};

#endif // USERACTIVITYREPORT_HPP
