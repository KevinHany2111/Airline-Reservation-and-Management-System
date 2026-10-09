#include "Flight.hpp"
#include "IdGenerator.hpp"
#include "Pilot.hpp"
#include "FlightAttendent.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

Flight::Flight(const std::string& flightNum,
               const std::string& o,
               const std::string& d,
               const std::string& ddt,
               const std::string& adt,
               const std::shared_ptr<Aircraft>& a,
               FlightStatus s,
               const std::string& g) :
               flightNumber(flightNum),
               origin(o),
               destination(d),
               departureDateTime(parseDateTime(ddt)),
               arrivalDateTime(parseDateTime(adt)),
               aircraft(a),
               status(s),
               gate(g), 
               pilot(nullptr),
               flightAttendant(nullptr) {}


// Getters
std::string Flight::getFlightNumber() const { return flightNumber; }
std::string Flight::getOrigin() const { return origin; }
std::string Flight::getDestination() const { return destination; }

std::string Flight::getDepartureDateTimeString() const { return formatDateTime(departureDateTime); }
std::string Flight::getArrivalDateTimeString() const { return formatDateTime(arrivalDateTime); }

FlightStatus Flight::getStatus() const { return status; }
std::string Flight::getStatusStr() const 
{
    switch(status)
    {
        case FlightStatus::Scheduled:  return "Scheduled";
        case FlightStatus::Delayed:    return "Delayed";
        case FlightStatus::Cancelled:  return "Cancelled";
        case FlightStatus::Completed:  return "Completed";
        default:                       return "Scheduled";
    }
}

std::shared_ptr<Aircraft> Flight::getAircraft() const{ return aircraft; }
std::string Flight::getGate() const { return gate; }

std::shared_ptr<ICrewMember> Flight::getPilot() const { return pilot; }
std::shared_ptr<ICrewMember> Flight::getFlightAttendant() const {return flightAttendant;}

std::chrono::system_clock::time_point Flight::getDepartureTimePoint() const { return departureDateTime; }
std::chrono::system_clock::time_point Flight::getArrivalTimePoint() const { return arrivalDateTime; }

int Flight::getAvailableSeatsCount() const 
{
    if(!aircraft) { return 0; }
    int count = 0;
    const auto& seats = aircraft->getSeats();
    for(const auto& seat: seats )
    {
        if(seat && seat->isAvailableSeat())
        {
            count++;
        }
    }
    return count;
}

// Private Setter for Adminstrator
void Flight::setStatus(FlightStatus newStatus) { status = newStatus; }
void Flight::setGate(const std::string& g) { gate = g; }

void Flight::assignPilot(const std::shared_ptr<ICrewMember>& p) { pilot = p; }
void Flight::assignFlightAttendant(const std::shared_ptr<ICrewMember>& f) { flightAttendant = f; }


// Utilities
void Flight::displaySeats(SeatClass cls) const
{
    if(!aircraft)
    {
        std::cout << "No Aircraft assigned to this flight" << std::endl;
        return;
    }
    aircraft ->displaySeatMap(cls);
}

// Parse "YYYY-MM-DD HH:MM" into time_point (local time)
std::chrono::system_clock::time_point Flight::parseDateTime(const std::string& dt)
{
    std::tm tm = {};
    std::istringstream ss(dt);
    ss >> std::get_time(&tm , "%Y-%m-%d %H:%M");
    if(ss.fail())
    {
        // Check this condition 
        return std::chrono::system_clock::now();
    }
    //tm is in local time -> convert to time_t
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

// Format "YYYY-MM-DD HH:MM"
std::string Flight::formatDateTime(const std::chrono::system_clock::time_point& tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm buf;
//#ifdef _WIN32
    localtime_s(&buf, &t);
//#else
//    localtime_r(&t, &buf);
//#endif
    std::ostringstream oss;
    oss << std::put_time(&buf, "%Y-%m-%d %H:%M");
    return oss.str();
}


// Format "HH:MM"
std::string Flight::formatTimeHM(const std::chrono::system_clock::time_point& tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm buf;
//#ifdef _WIN32
    localtime_s(&buf, &t);
//#else
//    localtime_r(&t, &buf);
//#endif
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << buf.tm_hour
        << ":" << std::setw(2) << std::setfill('0') << buf.tm_min;
    return oss.str();
}

// Helper methods for serialization and serialization (data base dealing)

json Flight::toJson() const
{
    json j;

    // --- Basic flight info ---
    j["flightNumber"] = flightNumber;
    j["origin"] = origin;
    j["destination"] = destination;
    j["departure"] = getDepartureDateTimeString();
    j["arrival"] = getArrivalDateTimeString();

    // --- Status ---
    j["status"] = getStatusStr();

    // --- Gate ---
    j["gate"] = gate;

    // --- Crew Info ---
    if (pilot)
    {
        j["pilot"] = {
            {"name", pilot->getName()},
            {"id", pilot->getId()}
        };
    }

    if (flightAttendant)
    {
        j["flightAttendant"] = {
            {"name", flightAttendant->getName()},
            {"id", flightAttendant->getId()}
        };
    }

    // --- Aircraft (kept at the end) ---
    if (aircraft)
    {
        j["aircraft"] = aircraft->toJson();
    }
    else
    {
        j["aircraftModel"] = "Unknown";
    }

    return j;
}

Flight Flight::fromJson(const json& j)
{
    // --- Aircraft (parsed last) ---
    std::shared_ptr<Aircraft> ac;
    if (j.contains("aircraft") && j["aircraft"].is_object())
    {
        ac = Aircraft::fromJson(j.at("aircraft"));
    }
    else
    {
        ac = Aircraft::fromJson({
            {"modelName", j.at("aircraftModel")},
            {"totalSeats", 180}
        });
    }

    // --- Status ---
    std::string statusStr = j.at("status");
    FlightStatus st;
    if (statusStr == "Scheduled")        st = FlightStatus::Scheduled;
    else if (statusStr == "Delayed")     st = FlightStatus::Delayed;
    else if (statusStr == "Cancelled")   st = FlightStatus::Cancelled;
    else if (statusStr == "Completed")   st = FlightStatus::Completed;
    else                                 st = FlightStatus::Scheduled;

    // --- Construct Flight ---
    Flight f(
        j.at("flightNumber"),
        j.at("origin"),
        j.at("destination"),
        j.at("departure"),
        j.at("arrival"),
        ac,
        st,
        j.at("gate")
    );

    // --- Crew Info ---
    if (j.contains("pilot") && j["pilot"].is_object())
    {
        const auto& p = j.at("pilot");
        f.pilot = std::make_shared<Pilot>(p.at("name"));
        // Reuse ID if provided
        if (p.contains("id"))
            f.pilot->ICrewMember::id = p.at("id");
    }

    if (j.contains("flightAttendant") && j["flightAttendant"].is_object())
    {
        const auto& a = j.at("flightAttendant");
        f.flightAttendant = std::make_shared<FlightAttendant>(a.at("name"));
        if (a.contains("id"))
            f.flightAttendant->ICrewMember::id = a.at("id");
    }

    return f;
}



/*
json Flight::toJson() const
{
    return {
        {"flightNumber", flightNumber},
        {"origin", origin},
        {"destination", destination},
        {"departureTime", formatDateTime(departureDateTime)},
        {"arrivalTime", formatDateTime(arrivalDateTime)},
        {"status", (status == FlightStatus::Scheduled ? "Scheduled" :
                    status == FlightStatus::Delayed ? "Delayed" : "Cancelled")},
        {"gate", gate},
        {"aircraftModel", aircraft ? aircraft->toJson() : ""},
    };
}

Flight Flight::fromJson(const json& j)
{
    // Load the full aircraft info if available, otherwise fallback to model name only
    std::shared_ptr<Aircraft> ac;

    if (j.contains("aircraft") && j["aircraft"].is_object()) {
        // New format: flight JSON includes full aircraft data
        ac = Aircraft::fromJson(j.at("aircraft"));
    } else {
        // Old format fallback: only model name and default seat count
        ac = Aircraft::fromJson({
            {"modelName", j.at("aircraftModel")},
            {"totalSeats", 180}
        });
    }

    // Parse flight status safely (and fix typo in "Scheduled")
    std::string statusStr = j.at("status");
    FlightStatus st;

    if (statusStr == "Scheduled")        { st = FlightStatus::Scheduled; }
    else if (statusStr == "Delayed")     { st = FlightStatus::Delayed; }
    else if (statusStr == "Cancelled")   { st = FlightStatus::Cancelled; }
    else if (statusStr == "Completed")   { st = FlightStatus::Completed; }
    else                                 { st = FlightStatus::Scheduled; } // fallback default


    // Construct and return the Flight
    Flight f(
        j.at("flightNumber"),
        j.at("origin"),
        j.at("destination"),
        j.at("departure"),
        j.at("arrival"),
        ac,
        st,
        j.at("gate")
    );
    return f;
}

*/
/*
Flight Flight::fromJson(const json& j)
{
    auto ac = Aircraft::fromJson({
    {"modelName", j.at("aircraftModel")},
    {"totalSeats", 180} // default fallback if totalSeats not stored
    });

    std::string statusStr = j.at("status");
    FlightStatus st;

    if(statusStr == "Shceduled") { st = FlightStatus::Scheduled; }
    else if(statusStr == "Delayed") { st = FlightStatus::Delayed; }
    else { st = FlightStatus::Cancelled; }

    Flight f(
        j.at("flightNumber"),
        j.at("origin"),
        j.at("destination"),
        j.at("departure"),
        j.at("arrival"),
        ac,
        st,
        j.at("gate")
    );

    return f;

}
*/