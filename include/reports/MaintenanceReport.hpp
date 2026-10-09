#ifndef MAINTENANCEREPORT_HPP
#define MAINTENANCEREPORT_HPP

#include "IReport.hpp"
#include <string>

class MaintenanceReport : public IReport
{
public:
    explicit MaintenanceReport();
    void generate(const std::string& monthYear) override;

private:
    std::string logsPath;
    static std::string extractMonthYearFromDateString(const std::string& dateStr);
};

#endif // MAINTENANCEREPORT_HPP
