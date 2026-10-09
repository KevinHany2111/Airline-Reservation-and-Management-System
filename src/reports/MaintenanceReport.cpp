#include "MaintenanceReport.hpp"
#include "RepositoryUtils.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <set>
#include <iomanip>
#include <map>

using json = nlohmann::json;

MaintenanceReport::MaintenanceReport() : logsPath("data/logs.json") {}


std::string MaintenanceReport::extractMonthYearFromDateString(const std::string& dateStr)
{
    if (dateStr.size() < 10) return "";
    try
    {
        std::string year = dateStr.substr(0,4);
        std::string month = dateStr.substr(5,2);
        return month + "-" + year;
    } catch (...) { return ""; }
}

void MaintenanceReport::generate(const std::string& monthYear)
{
    std::ostringstream ss;
    ss << "\n";
    ss << "Maintenance Report — " << monthYear << "\n";
    ss << "----------------------------------------\n";

    auto logs = RepositoryUtils::getAll(logsPath);

    bool logsHaveDates = true;
    int totalLogs = 0;
    std::set<std::string> aircraftServiced;
    std::set<std::string> engineers;
    std::vector<json> includedLogs;

    for (const auto &l : logs)
    {
        std::string dateCandidate;
        if (l.contains("date")) dateCandidate = l["date"].get<std::string>();
        else if (l.contains("timestamp")) dateCandidate = l["timestamp"].get<std::string>();
        else dateCandidate = "";

        std::string my = extractMonthYearFromDateString(dateCandidate);
        if (my.empty()) logsHaveDates = false;

        if (my.empty() || my == monthYear)
        {
            includedLogs.push_back(l);
            totalLogs++;
            if (l.contains("aircraftModel")) aircraftServiced.insert(l["aircraftModel"].get<std::string>());
            if (l.contains("engineer")) engineers.insert(l["engineer"].get<std::string>());
        }
    }

    if (!logsHaveDates)
        ss << "[Note] maintenance logs do not have date fields — report includes all logs.\n";

    ss << "Total Maintenance Logs: " << totalLogs << "\n";
    ss << "Aircraft Serviced: " << aircraftServiced.size() << "\n";
    ss << "Engineers Involved: " << engineers.size() << "\n";
    ss << "----------------------------------------\n";
    ss << "Maintenance Details:\n";

    int idx = 1;
    for (const auto &l : includedLogs)
    {
        std::string aircraft = l.value("aircraftModel", std::string("Unknown"));
        std::string engineer = l.value("engineer", std::string("Unknown"));
        std::string desc = l.value("description", std::string("-"));
        ss << idx++ << ". " << aircraft << " — " << engineer << " — " << desc << "\n";
    }
    ss << "----------------------------------------\n";

    std::string reportBody = ss.str();
    std::cout << "Generating Maintenance Report for " << monthYear << "...\n";
    std::cout << reportBody << std::endl;
    saveToTxt("data/reports/maintenanceReports.txt", reportBody);
    std::cout << "Maintenance report saved to data/reports/maintenanceReports.txt\n\n";
}
