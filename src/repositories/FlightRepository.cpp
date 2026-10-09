#include "FlightRepository.hpp"
#include "RepositoryUtils.hpp"
#include <iomanip>    // std::get_time, std::put_time
#include <ctime>
#include <sstream>
#include <iostream>

FlightRepository::FlightRepository(const std::string& path)
    : IRepository(path) {}

std::optional<json> FlightRepository::findById(const std::string& id) const
{
    return RepositoryUtils::getById(filePath, "flightNumber", id);
}

std::vector<json> FlightRepository::getAll() const
{
    return RepositoryUtils::getAll(filePath);
}

bool FlightRepository::add(const json& newObj)
{
    return RepositoryUtils::appendToFile(filePath, newObj);
}

bool FlightRepository::updateById(const std::string& id, const json& updatedObj)
{
    return RepositoryUtils::updateById(filePath, "flightNumber", id, updatedObj);
}

bool FlightRepository::removeById(const std::string& id)
{
    return RepositoryUtils::removeById(filePath, "flightNumber", id);
}


// helper: compute next day in "YYYY-MM-DD" format
std::string FlightRepository::nextDateString(const std::string& dateYMD)
{
    std::tm tm = {};
    std::istringstream iss(dateYMD + " 00:00");
    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    if (iss.fail())
    {
        // fallback: return original (so next-day won't match anything)
        return dateYMD;
    }
    std::time_t t = std::mktime(&tm);
    t += 24 * 60 * 60; // add one day
    std::tm nextTm;
#ifdef _WIN32
    localtime_s(&nextTm, &t);
#else
    localtime_r(&t, &nextTm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&nextTm, "%Y-%m-%d");
    return oss.str();
}

/*
std::vector<std::shared_ptr<Flight>> FlightRepository::findFlights(
    const std::string& origin,
    const std::string& destination,
    const std::string& departureDate) const
{
    std::vector<std::shared_ptr<Flight>> sameDay;
    std::vector<std::shared_ptr<Flight>> nextDay;
    json flightsData = JsonHandler::loadJson(filePath);

    if (!flightsData.is_array())
        return {};

    const std::string nextDate = nextDateString(departureDate);

    for (const auto& f : flightsData)
    {
        try
        {
            // create Flight object from json
            Flight flight = Flight::fromJson(f);

            if (flight.getOrigin() != origin) continue;
            if (flight.getDestination() != destination) continue;
            if (flight.getStatus() != FlightStatus::Scheduled) continue;
            if (!flight.getAircraft() || !flight.getAircraft()->hasAvailableSeat()) continue;

            // get YYYY-MM-DD string of departure
            std::string depFull = Flight::formatDateTime(flight.getDepartureTimePoint()); // "YYYY-MM-DD HH:MM"
            std::string depDate = depFull.size() >= 10 ? depFull.substr(0, 10) : "";

            if (depDate == departureDate)
            {
                sameDay.push_back(std::make_shared<Flight>(flight));
            }
            else if (depDate == nextDate)
            {
                nextDay.push_back(std::make_shared<Flight>(flight));
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "[FlightRepository] Error parsing flight record: " << e.what() << std::endl;
        }
    }

    // Return same-day first then next-day appended
    std::vector<std::shared_ptr<Flight>> result;
    result.reserve(sameDay.size() + nextDay.size());
    for (auto &ptr : sameDay) result.push_back(ptr);
    for (auto &ptr : nextDay) result.push_back(ptr);
    return result;
}
*/
std::vector<std::shared_ptr<Flight>> FlightRepository::findFlights(
    const std::string& origin,
    const std::string& destination,
    const std::string& date) const
{
    std::vector<std::shared_ptr<Flight>> matchingFlights;

    try
    {
        json flightsJson = JsonHandler::loadJson("data/flights.json");

        if (!flightsJson.is_array())
        {
            std::cerr << "Invalid flights.json format.\n";
            return matchingFlights;
        }

        for (const auto& flightData : flightsJson)
        {
            try
            {
                // Build a Flight object from JSON (by value)
                Flight flight = Flight::fromJson(flightData);

                bool originMatch = flight.getOrigin() == origin;
                bool destMatch   = flight.getDestination() == destination;
                bool dateMatch   = true;

                if (!date.empty())
                {
                    std::string depTime = flight.getDepartureDateTimeString();
                    if (depTime.size() >= date.size())
                        dateMatch = depTime.substr(0, date.size()) == date;
                    else
                        dateMatch = false;
                }

                if (originMatch && destMatch && dateMatch)
                    matchingFlights.push_back(std::make_shared<Flight>(flight));
            }
            catch (const std::exception& e)
            {
                std::cerr << "Skipping invalid flight record: " << e.what() << '\n';
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading flights.json: " << e.what() << '\n';
    }

    return matchingFlights;
}

bool FlightRepository::saveFlight(const Flight& flight)
{
    json updatedJson = flight.toJson();
    return updateById(flight.getFlightNumber(), updatedJson);
}
