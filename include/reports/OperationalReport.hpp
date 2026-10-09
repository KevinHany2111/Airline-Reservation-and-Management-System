#ifndef OPERATIONALREPORT_HPP
#define OPERATIONALREPORT_HPP

#include "IReport.hpp"

class OperationalReport : public IReport
{
public:
    // Provide paths for flights and reservations JSON files
    OperationalReport();

    void generate(const std::string& monthYear) override;

private:
    std::string flightsPath;
    std::string reservationsPath;

    // helpers
    static std::string extractMonthYearFromDateString(const std::string& dateStr);
    static std::string toLower(const std::string& s);
};

#endif // OPERATIONALREPORT_HPP
