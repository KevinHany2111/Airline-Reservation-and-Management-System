#include "UserActivityReport.hpp"
#include "RepositoryUtils.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>

using json = nlohmann::json;

UserActivityReport::UserActivityReport() :  reservationsPath("data/reservations.json"),
                                            passengersPath("data/passengers.json"),
                                            agentsPath("data/bookingAgents.json"),
                                            adminsPath("data/administrators.json") {}

std::string UserActivityReport::extractMonthYearFromDateString(const std::string& dateStr)
{
    if (dateStr.size() < 10) return "";
    try
    {
        std::string year = dateStr.substr(0,4);
        std::string month = dateStr.substr(5,2);
        return month + "-" + year;
    } catch (...) { return ""; }
}

void UserActivityReport::generate(const std::string& monthYear)
{
    std::ostringstream ss;
    ss << "\n";
    ss << "User Activity Report — " << monthYear << "\n";
    ss << "----------------------------------------\n";

    auto reservations = RepositoryUtils::getAll(reservationsPath);
    auto passengers = RepositoryUtils::getAll(passengersPath);
    auto agents = RepositoryUtils::getAll(agentsPath);
    auto admins = RepositoryUtils::getAll(adminsPath);

    // totals
    int totalPassengers = static_cast<int>(passengers.size());
    int totalAgents = static_cast<int>(agents.size());
    int totalAdmins = static_cast<int>(admins.size());

    // active passengers (with reservations in month)
    std::set<std::string> activePassengers;
    bool reservationsHaveDates = true;
    int totalReservationsThisMonth = 0;
    std::map<std::string,int> passengerReservationCounts;

    for (const auto &r : reservations)
    {
        // Extract reservation date (nested flight or direct field)
        std::string dateCandidate;
        if (r.contains("flight"))
        {
            auto &f = r["flight"];
            if (f.contains("departure")) dateCandidate = f["departure"].get<std::string>();
            else if (f.contains("departureDateTime")) dateCandidate = f["departureDateTime"].get<std::string>();
        }
        if (dateCandidate.empty())
        {
            if (r.contains("date")) dateCandidate = r["date"].get<std::string>();
            else if (r.contains("departure")) dateCandidate = r["departure"].get<std::string>();
        }

        std::string my = extractMonthYearFromDateString(dateCandidate);
        if (my.empty()) reservationsHaveDates = false;

        if (my.empty() || my == monthYear)
        {
            totalReservationsThisMonth++;
            std::string pid = r.value("passengerId", r.value("passenger", std::string("?")));
            activePassengers.insert(pid);
            passengerReservationCounts[pid]++;
        }
    }

    if (!reservationsHaveDates)
        ss << "[Note] reservations.json lacks consistent date fields; counts include all reservations.\n";

    // Top active passengers: convert map to vector and sort
    std::vector<std::pair<std::string,int>> vec;
    for (auto &p : passengerReservationCounts) vec.push_back(p);
    std::sort(vec.begin(), vec.end(), [](auto &a, auto &b){ return a.second > b.second; });

    ss << "Total Passengers: " << totalPassengers << "\n";
    ss << "Active Passengers (with reservations): " << activePassengers.size() << "\n";
    ss << "Booking Agents: " << totalAgents << "\n";
    ss << "Administrators: " << totalAdmins << "\n";
    ss << "Total Reservations This Month: " << totalReservationsThisMonth << "\n";
    ss << "----------------------------------------\n";
    ss << "Top Active Passengers:\n";

    int limit = std::min<size_t>(5, vec.size());
    for (int i = 0; i < limit; ++i)
    {
        ss << (i+1) << ". " << vec[i].first << " — " << vec[i].second << " Reservations\n";
    }
    ss << "----------------------------------------\n";

    std::string reportBody = ss.str();

    std::cout << "Generating User Activity Report for " << monthYear << "...\n";
    std::cout << reportBody << std::endl;
    saveToTxt("data/reports/userActivityReports.txt", reportBody);
    std::cout << "User activity report saved to data/reports/userActivityReports.txt\n\n";
}
