#include "OperationalReport.hpp"
#include "RepositoryUtils.hpp"
#include "JsonHandler.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <map>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <algorithm>


using json = nlohmann::json;

OperationalReport::OperationalReport() : flightsPath("data/flights.json"), reservationsPath("data/reservations.json") {}


std::string OperationalReport::toLower(const std::string& s)
{
    std::string out = s;
    for (auto &c : out) c = static_cast<char>(std::tolower((unsigned char)c));
    return out;
}

std::string OperationalReport::extractMonthYearFromDateString(const std::string& dateStr)
{
    // Accept formats like "YYYY-MM-DD HH:MM" or "YYYY-MM-DD"
    // Return "MM-YYYY"
    if (dateStr.size() < 10) return "";
    // year = substr(0,4), month = substr(5,2)
    try
    {
        std::string year = dateStr.substr(0,4);
        std::string month = dateStr.substr(5,2);
        if (month.size() == 2 && year.size() == 4)
            return month + "-" + year;
    }
    catch (...)
    {
        return "";
    }
    return "";
}



void OperationalReport::generate(const std::string& monthYear)
{
    std::cout << "Generating Operational Report for " << monthYear << "...\n";

    json flightsData = JsonHandler::loadJson("data/flights.json");
    json reservationsData = JsonHandler::loadJson("data/reservations.json");

    if (!flightsData.is_array() || flightsData.empty())
    {
        std::cout << "No flight data found.\n";
        return;
    }

    // Parse month and year (MM-YYYY)
    int month = std::stoi(monthYear.substr(0, 2));
    int year  = std::stoi(monthYear.substr(3, 4));

    auto parseMonthYear = [](const std::string& dateStr, int& outMonth, int& outYear) -> bool {
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
        if (ss.fail()) return false;
        outMonth = tm.tm_mon + 1;
        outYear = tm.tm_year + 1900;
        return true;
    };

    // Filter flights in given month
    std::vector<json> filteredFlights;
    for (const auto& flight : flightsData)
    {
        int fMonth = 0, fYear = 0;
        std::string dep = flight.value("departure", "");
        if (!dep.empty() && parseMonthYear(dep, fMonth, fYear))
        {
            if (fMonth == month && fYear == year)
                filteredFlights.push_back(flight);
        }
    }

    // Filter reservations in given month
    std::vector<json> filteredReservations;
    if (reservationsData.is_array())
    {
        for (const auto& res : reservationsData)
        {
            int rMonth = 0, rYear = 0;
            std::string dep = res.value("departureTime", "");
            if (!dep.empty() && parseMonthYear(dep, rMonth, rYear))
            {
                if (rMonth == month && rYear == year)
                    filteredReservations.push_back(res);
            }
        }
    }

    // ---- Aggregate statistics ----
    int totalFlights = static_cast<int>(filteredFlights.size());
    int completedFlights = 0, delayedFlights = 0, canceledFlights = 0, scheduledFlights = 0;
    int totalReservations = 0;
    double totalRevenue = 0.0;

    for (const auto& flight : filteredFlights)
    {
        std::string status = toLower(flight.value("status", "Unknown"));
        if (status.find("del") != std::string::npos)
            delayedFlights++;
        else if (status.find("cancel") != std::string::npos)
            canceledFlights++;
        else if (status.find("comp") != std::string::npos)
            completedFlights++;
        else if (status.find("sched") != std::string::npos)
            scheduledFlights++;
    }

    for (const auto& res : filteredReservations)
    {
        bool isCancelled = res.value("cancelled", false);
        if (!isCancelled)
        {
            totalReservations++;
            if (res.contains("payment") && res["payment"].contains("amount"))
                totalRevenue += res["payment"]["amount"].get<double>();
        }
    }

    // ---- Build report text ----
    std::ostringstream report;
    report << "Operational Report — " << monthYear << "\n";
    report << "----------------------------------------\n";

    if (reservationsData.empty() || !reservationsData.is_array())
        report << "[Note] reservations.json lacks consistent date fields.\n";

    report << "Total Flights: " << totalFlights << "\n";
    report << "Flights Completed: " << completedFlights << "\n";
    report << "Flights Delayed: " << delayedFlights << "\n";
    report << "Flights Canceled: " << canceledFlights << "\n";
    report << "Upcoming Flights (Scheduled): " << scheduledFlights << "\n";
    report << "Total Reservations Made: " << totalReservations << "\n";
    report << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << "\n";
    report << "----------------------------------------\n";
    report << "Flight Performance:\n";

    int index = 1;
    for (const auto& flight : filteredFlights)
    {
        std::string fNum = flight.value("flightNumber", "Unknown");
        std::string status = flight.value("status", "Unknown");

        // Count valid reservations for this flight (non-cancelled)
        int flightResCount = 0;
        double flightRevenue = 0.0;
        for (const auto& res : filteredReservations)
        {
            if (res.value("flightNumber", "") == fNum && !res.value("cancelled", false))
            {
                flightResCount++;
                if (res.contains("payment") && res["payment"].contains("amount"))
                    flightRevenue += res["payment"]["amount"].get<double>();
            }
        }

        report << index++ << ". " << fNum << " — " << status
               << " (" << flightResCount << " Bookings, $"
               << std::fixed << std::setprecision(2) << flightRevenue << ")\n";
    }

    report << "----------------------------------------\n";

    // ---- File handling ----
    std::filesystem::create_directories("data/reports");
    std::string outputPath = "data/reports/operationalReports.txt";

    // Load old reports if any
    std::vector<std::string> existingReports;
    {
        std::ifstream in(outputPath);
        std::string block, line;
        while (std::getline(in, line))
        {
            if (line.rfind("Operational Report — ", 0) == 0)
            {
                if (!block.empty()) existingReports.push_back(block);
                block = line + "\n";
            }
            else
                block += line + "\n";
        }
        if (!block.empty()) existingReports.push_back(block);
    }

    // Remove old report for this same month
    existingReports.erase(std::remove_if(existingReports.begin(), existingReports.end(),
        [&](const std::string& block) { return block.find("Operational Report — " + monthYear) != std::string::npos; }),
        existingReports.end());

    // Add new one and sort descending by month-year
    existingReports.push_back(report.str());
    std::sort(existingReports.begin(), existingReports.end(), [](const std::string& a, const std::string& b)
    {
        auto extractMY = [](const std::string& rep) -> std::pair<int,int> {
            size_t pos = rep.find("Operational Report — ");
            if (pos == std::string::npos) return {0,0};
            std::string part = rep.substr(pos + 22, 7); // "MM-YYYY"
            int m = std::stoi(part.substr(0,2));
            int y = std::stoi(part.substr(3,4));
            return {y, m};
        };
        auto [ya, ma] = extractMY(a);
        auto [yb, mb] = extractMY(b);
        if (ya != yb) return ya > yb;
        return ma > mb;
    });

    // Save all reports back
    std::ofstream out(outputPath, std::ios::trunc);
    for (const auto& rep : existingReports)
        out << rep << "\n";

    out.close();

    // ---- Print only this month’s report ----
    std::cout << "\n" << report.str() << std::endl;
    std::cout << "Operational report saved to " << outputPath << "\n";
}




/*
void OperationalReport::generate(const std::string& monthYear)
{
    std::cout << "Generating Operational Report for " << monthYear << "...\n";

    json flightsData = JsonHandler::loadJson("data/flights.json");
    json reservationsData = JsonHandler::loadJson("data/reservations.json");

    if (!flightsData.is_array() || flightsData.empty()) {
        std::cout << "No flight data found.\n";
        return;
    }

    // Parse MM-YYYY
    int month = std::stoi(monthYear.substr(0, 2));
    int year  = std::stoi(monthYear.substr(3, 4));

    auto parseMonthYear = [](const std::string& dateStr, int& outMonth, int& outYear) -> bool {
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
        if (ss.fail()) return false;
        outMonth = tm.tm_mon + 1;
        outYear = tm.tm_year + 1900;
        return true;
    };

    // Filter flights in this month
    std::vector<json> filteredFlights;
    for (const auto& flight : flightsData) {
        int fMonth = 0, fYear = 0;
        std::string dep = flight.value("departure", "");
        if (!dep.empty() && parseMonthYear(dep, fMonth, fYear)) {
            if (fMonth == month && fYear == year)
                filteredFlights.push_back(flight);
        }
    }

    // Filter reservations in this month
    std::vector<json> filteredReservations;
    if (reservationsData.is_array()) {
        for (const auto& res : reservationsData) {
            int rMonth = 0, rYear = 0;
            std::string dep = res.value("departureTime", "");
            if (!dep.empty() && parseMonthYear(dep, rMonth, rYear)) {
                if (rMonth == month && rYear == year)
                    filteredReservations.push_back(res);
            }
        }
    }

    // Stats
    int totalFlights = static_cast<int>(filteredFlights.size());
    int completedFlights = 0, delayedFlights = 0, canceledFlights = 0;
    int totalReservations = static_cast<int>(filteredReservations.size());
    double totalRevenue = 0.0;

    for (const auto& flight : filteredFlights) {
        std::string status = flight.value("status", "Unknown");
        if (status == "Completed") completedFlights++;
        else if (status == "Delayed") delayedFlights++;
        else if (status == "Canceled") canceledFlights++;
    }

    for (const auto& res : filteredReservations) {
        if (res.contains("payment") && res["payment"].contains("amount"))
            totalRevenue += res["payment"]["amount"].get<double>();
    }

    // Build current month report
    std::ostringstream report;
    report << "Operational Report — " << monthYear << "\n";
    report << "----------------------------------------\n";
    report << "Total Flights Scheduled: " << totalFlights << "\n";
    report << "Flights Completed: " << completedFlights << "\n";
    report << "Flights Delayed: " << delayedFlights << "\n";
    report << "Flights Canceled: " << canceledFlights << "\n";
    report << "Total Reservations Made: " << totalReservations << "\n";
    report << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << "\n";
    report << "----------------------------------------\n";
    report << "Flight Performance:\n";
    int idx = 1;
    for (const auto& flight : filteredFlights) {
        std::string fNum = flight.value("flightNumber", "Unknown");
        std::string status = flight.value("status", "Unknown");
        int count = 0; double revenue = 0.0;
        for (const auto& res : filteredReservations) {
            if (res.value("flightNumber", "") == fNum) {
                count++;
                if (res.contains("payment") && res["payment"].contains("amount"))
                    revenue += res["payment"]["amount"].get<double>();
            }
        }
        report << idx++ << ". " << fNum << " — " << status
               << " (" << count << " Bookings, $" << std::fixed << std::setprecision(2) << revenue << ")\n";
    }
    report << "----------------------------------------\n\n";

    std::string currentReportText = report.str();

    // === File Handling: replace same month, keep others sorted descendingly ===
    std::filesystem::create_directories("data/reports");
    std::string outputPath = "data/reports/operationalReports.txt";

    // Read all existing reports (split by delimiter)
    std::vector<std::string> allReports;
    if (std::filesystem::exists(outputPath)) {
        std::ifstream in(outputPath);
        std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        in.close();

        std::stringstream ss(content);
        std::string block;
        while (std::getline(ss, block, '\f')) {  // use form-feed (\f) as separator
            if (!block.empty()) allReports.push_back(block);
        }
    }

    // Remove old report for same month
    allReports.erase(
        std::remove_if(allReports.begin(), allReports.end(), [&](const std::string& r) {
            return r.find("Operational Report — " + monthYear) != std::string::npos;
        }),
        allReports.end()
    );

    // Add new one
    allReports.push_back(currentReportText);

    // Sort descendingly by month-year (most recent first)
    std::sort(allReports.begin(), allReports.end(), [](const std::string& a, const std::string& b) {
        auto getKey = [](const std::string& r) {
            size_t pos = r.find("Operational Report — ");
            if (pos == std::string::npos) return std::string("00-0000");
            return r.substr(pos + 22, 7);
        };
        return getKey(a) > getKey(b);
    });

    // Write all reports separated by \f
    std::ofstream out(outputPath, std::ios::trunc);
    for (size_t i = 0; i < allReports.size(); ++i) {
        out << allReports[i];
        if (i != allReports.size() - 1)
            out << "\f";
    }
    out.close();

    // ✅ Print only the generated report for this month
    std::cout << "\n" << currentReportText ;
    std::cout << "Report saved and sorted in " << outputPath << "\n";
}


void OperationalReport::generate(const std::string& monthYear)
{
    std::ostringstream ss;
    ss << "\n";
    ss << "Operational Report — " << monthYear << "\n";
    ss << "----------------------------------------\n";

    // Load flights and reservations
    auto flights = RepositoryUtils::getAll(flightsPath);
    auto reservations = RepositoryUtils::getAll(reservationsPath);

    bool flightsHaveDates = true;
    bool reservationsHaveDates = true;

    // Per-flight stats: flightNumber -> (bookings, revenue, status)
    struct FlightStat { int bookings = 0; double revenue = 0.0; std::string status = "Unknown"; };
    std::map<std::string, FlightStat> flightStats;

    int totalFlightsScheduled = 0;
    int flightsCompleted = 0;
    int flightsDelayed = 0;
    int flightsCancelled = 0;

    // Count flights in month
    for (const auto& f : flights)
    {
        // look for potential date fields
        std::string dateCandidate;
        if (f.contains("departure")) dateCandidate = f["departure"].get<std::string>();
        else if (f.contains("departureDateTime")) dateCandidate = f["departureDateTime"].get<std::string>();
        else if (f.contains("departureTime")) dateCandidate = f["departureTime"].get<std::string>();
        else if (f.contains("date")) dateCandidate = f["date"].get<std::string>();
        else dateCandidate = "";

        std::string my = extractMonthYearFromDateString(dateCandidate);
        if (my.empty()) flightsHaveDates = false;

        if (!my.empty() && my == monthYear)
        {
            totalFlightsScheduled++;
            std::string fn = f.value("flightNumber", std::string("?"));
            std::string status = f.value("status", std::string("Unknown"));
            std::string ls = toLower(status);
            if (ls.find("del") != std::string::npos) flightsDelayed++;
            else if (ls.find("cancel") != std::string::npos) flightsCancelled++;
            else flightsCompleted++; // treat non-explicit as completed for this month
            flightStats[fn].status = status;
        }
    }

    // Count reservations and revenue for month and per-flight
    int totalReservationsMade = 0;
    double totalRevenue = 0.0;

    for (const auto& r : reservations)
    {
        // try to find date in reservation or nested flight
        std::string dateCandidate;
        if (r.contains("flight"))
        {
            const auto& nested = r["flight"];
            if (nested.contains("departure")) dateCandidate = nested["departure"].get<std::string>();
            else if (nested.contains("departureDateTime")) dateCandidate = nested["departureDateTime"].get<std::string>();
            else if (nested.contains("departureTime")) dateCandidate = nested["departureTime"].get<std::string>();
        }
        if (dateCandidate.empty())
        {
            if (r.contains("departure")) dateCandidate = r["departure"].get<std::string>();
            else if (r.contains("departureDateTime")) dateCandidate = r["departureDateTime"].get<std::string>();
            else if (r.contains("date")) dateCandidate = r["date"].get<std::string>();
        }

        std::string my = extractMonthYearFromDateString(dateCandidate);
        if (my.empty()) reservationsHaveDates = false;

        if (my.empty() || my == monthYear)
        {
            // Count it (if my empty -> include in totals but will note inability to filter)
            totalReservationsMade++;

            // amount
            double amt = 0.0;
            if (r.contains("payment") && r["payment"].contains("amount"))
            {
                try { amt = r["payment"]["amount"].get<double>(); }
                catch(...) { amt = r["payment"]["amount"].get<int>(); }
            }
            else if (r.contains("amount"))
            {
                try { amt = r["amount"].get<double>(); } catch(...) { amt = r["amount"].get<int>(); }
            }
            totalRevenue += amt;

            // per-flight aggregation
            std::string fn = r.value("flightNumber", std::string("?"));
            if (fn == "?" && r.contains("flight") && r["flight"].contains("flightNumber"))
                fn = r["flight"]["flightNumber"].get<std::string>();

            flightStats[fn].bookings++;
            flightStats[fn].revenue += amt;
        }
    }

    // If filtering by month wasn't possible for some source, indicate
    if (!flightsHaveDates)
        ss << "[Note] flights.json lacks consistent departure date fields; counts include all flights.\n";
    if (!reservationsHaveDates)
        ss << "[Note] reservations.json lacks consistent date fields; counts include all reservations.\n";

    ss << "Total Flights Scheduled: " << totalFlightsScheduled << "\n";
    ss << "Flights Completed: " << flightsCompleted << "\n";
    ss << "Flights Delayed: " << flightsDelayed << "\n";
    ss << "Flights Canceled: " << flightsCancelled << "\n";
    ss << "Total Reservations Made: " << totalReservationsMade << "\n";
    ss << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << "\n";
    ss << "----------------------------------------\n";
    ss << "Flight Performance:\n";

    // Sort flights by bookings desc for listing
    std::vector<std::pair<std::string, FlightStat>> vec;
    for (auto &p : flightStats) vec.push_back(p);
    std::sort(vec.begin(), vec.end(), [](auto &a, auto &b){
        return a.second.bookings > b.second.bookings;
    });

    int idx = 1;
    for (auto &p : vec)
    {
        ss << idx++ << ". " << p.first << " — " << p.second.status
           << " (" << p.second.bookings << " Bookings, $"
           << std::fixed << std::setprecision(2) << p.second.revenue << ")\n";
    }

    std::string reportBody = ss.str();

    // Print brief console message and save to file
    ss << "----------------------------------------\n";
    std::cout << "Generating Operational Report for " << monthYear << "...\n";
    saveToTxt("data/reports/operationalReports.txt", reportBody);
    std::cout << reportBody << std::endl;
    std::cout << "Operational report saved to data/reports/operationalReports.txt\n";
}


void OperationalReport::generate(const std::string& monthYear)
{
    std::ostringstream ss;
    ss << "\n";
    ss << "Operational Report — " << monthYear << "\n";
    ss << "----------------------------------------\n";

    // Load data
    auto flights = RepositoryUtils::getAll(flightsPath);
    auto reservations = RepositoryUtils::getAll(reservationsPath);

    bool flightsHaveDates = true;
    bool reservationsHaveDates = true;

    struct FlightStat {
        int bookings = 0;
        double revenue = 0.0;
        std::string status = "Unknown";
    };

    std::map<std::string, FlightStat> flightStats;

    int totalFlightsScheduled = 0;
    int flightsCompleted = 0;
    int flightsDelayed = 0;
    int flightsCancelled = 0;

    // -----------------------------
    // 1️⃣ Process Flights
    // -----------------------------
    for (const auto& f : flights)
    {
        std::string dateCandidate;
        if (f.contains("departure")) dateCandidate = f["departure"].get<std::string>();
        else if (f.contains("departureDateTime")) dateCandidate = f["departureDateTime"].get<std::string>();
        else if (f.contains("departureTime")) dateCandidate = f["departureTime"].get<std::string>();
        else if (f.contains("date")) dateCandidate = f["date"].get<std::string>();

        std::string my = extractMonthYearFromDateString(dateCandidate);
        if (my.empty()) flightsHaveDates = false;

        // ✅ Only include flights for the selected month
        if (my == monthYear)
        {
            totalFlightsScheduled++;
            std::string fn = f.value("flightNumber", std::string("?"));
            std::string status = f.value("status", std::string("Unknown"));
            std::string ls = toLower(status);

            if (ls.find("del") != std::string::npos) flightsDelayed++;
            else if (ls.find("cancel") != std::string::npos) flightsCancelled++;
            else flightsCompleted++; // treat non-explicit as completed

            flightStats[fn].status = status;
        }
    }

    // -----------------------------
    // 2️⃣ Process Reservations
    // -----------------------------
    int totalReservationsMade = 0;
    double totalRevenue = 0.0;

    for (const auto& r : reservations)
    {
        std::string dateCandidate;

        // Try nested flight date
        if (r.contains("flight"))
        {
            const auto& nested = r["flight"];
            if (nested.contains("departure")) dateCandidate = nested["departure"].get<std::string>();
            else if (nested.contains("departureDateTime")) dateCandidate = nested["departureDateTime"].get<std::string>();
            else if (nested.contains("departureTime")) dateCandidate = nested["departureTime"].get<std::string>();
        }

        // Fallback to top-level date fields
        if (dateCandidate.empty())
        {
            if (r.contains("departure")) dateCandidate = r["departure"].get<std::string>();
            else if (r.contains("departureDateTime")) dateCandidate = r["departureDateTime"].get<std::string>();
            else if (r.contains("date")) dateCandidate = r["date"].get<std::string>();
        }

        std::string my = extractMonthYearFromDateString(dateCandidate);
        if (my.empty()) reservationsHaveDates = false;

        // ✅ Include only reservations for the selected month
        if (my == monthYear)
        {
            totalReservationsMade++;

            double amt = 0.0;
            if (r.contains("payment") && r["payment"].contains("amount"))
            {
                try { amt = r["payment"]["amount"].get<double>(); }
                catch(...) { amt = r["payment"]["amount"].get<int>(); }
            }
            else if (r.contains("amount"))
            {
                try { amt = r["amount"].get<double>(); }
                catch(...) { amt = r["amount"].get<int>(); }
            }

            totalRevenue += amt;

            std::string fn = r.value("flightNumber", std::string("?"));
            if (fn == "?" && r.contains("flight") && r["flight"].contains("flightNumber"))
                fn = r["flight"]["flightNumber"].get<std::string>();

            // ✅ Only add stats if flight was already included
            if (flightStats.find(fn) != flightStats.end())
            {
                flightStats[fn].bookings++;
                flightStats[fn].revenue += amt;
            }
        }
    }

    // -----------------------------
    // 3️⃣ Report Summary
    // -----------------------------
    if (!flightsHaveDates)
        ss << "[Note] flights.json lacks consistent departure date fields.\n";
    if (!reservationsHaveDates)
        ss << "[Note] reservations.json lacks consistent date fields.\n";

    ss << "Total Flights Scheduled: " << totalFlightsScheduled << "\n";
    ss << "Flights Completed: " << flightsCompleted << "\n";
    ss << "Flights Delayed: " << flightsDelayed << "\n";
    ss << "Flights Canceled: " << flightsCancelled << "\n";
    ss << "Total Reservations Made: " << totalReservationsMade << "\n";
    ss << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << "\n";
    ss << "----------------------------------------\n";
    ss << "Flight Performance:\n";

    // Sort by bookings
    std::vector<std::pair<std::string, FlightStat>> vec(flightStats.begin(), flightStats.end());
    std::sort(vec.begin(), vec.end(), [](auto &a, auto &b) {
        return a.second.bookings > b.second.bookings;
    });

    int idx = 1;
    for (auto &p : vec)
    {
        ss << idx++ << ". " << p.first << " — " << p.second.status
           << " (" << p.second.bookings << " Bookings, $"
           << std::fixed << std::setprecision(2) << p.second.revenue << ")\n";
    }
    ss << "----------------------------------------\n";

    // -----------------------------
    // 4️⃣ Output & Save
    // -----------------------------
    std::string reportBody = ss.str();
    saveToTxt("data/reports/operationalReports.txt", reportBody);

    std::cout << "Generating Operational Report for " << monthYear << "...\n";
    std::cout << reportBody << std::endl;
    std::cout << "Operational report saved to data/reports/operationalReports.txt\n";
}
*/
