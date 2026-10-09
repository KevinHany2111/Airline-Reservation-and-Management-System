#ifndef FLIGHT_HPP
#define FLIGHT_HPP

#include <string>
#include <memory>
#include <chrono>
#include <nlohmann/json.hpp>
#include "Aircraft.hpp"
#include "Seat.hpp"
#include "ICrewMember.hpp"

using json = nlohmann::json;

class Administrator;
class Menu;                        // Forward Declaration

enum class FlightStatus {Scheduled, Delayed, Cancelled, Completed };

class Flight
{
    friend class Menu;
    friend class Administrator;

private:
    std::string flightNumber;
    std::string origin;
    std::string destination;

    // Use chrono time_points internally
    std::chrono::system_clock::time_point departureDateTime;
    std::chrono::system_clock::time_point arrivalDateTime;

    FlightStatus status;
    std::shared_ptr<Aircraft> aircraft;
    std::string gate;

    std::shared_ptr<ICrewMember> pilot;
    std::shared_ptr<ICrewMember> flightAttendant;

    void assignPilot(const std::shared_ptr<ICrewMember>& p);
    void assignFlightAttendant(const std::shared_ptr<ICrewMember>& f);

    void setStatus(FlightStatus newStatus) ;
    void setGate(const std::string& g);

public:
    Flight(const std::string& flightNum,
           const std::string& o,
           const std::string& d,
           const std::string& ddt,
           const std::string& adt,
           const std::shared_ptr<Aircraft>& a,
           FlightStatus s, 
           const std::string& g = "A1");

    // Getters
    std::string getFlightNumber() const;
    std::string getOrigin() const;
    std::string getDestination() const;
    std::string getDepartureDateTimeString() const;
    std::string getArrivalDateTimeString() const;
    std::chrono::system_clock::time_point getDepartureTimePoint() const;
    std::chrono::system_clock::time_point getArrivalTimePoint() const;
    FlightStatus getStatus() const;
    std::string getStatusStr() const;
    std::shared_ptr<Aircraft> getAircraft() const;
    std::string getGate() const;
    std::shared_ptr<ICrewMember> getPilot() const;
    std::shared_ptr<ICrewMember> getFlightAttendant() const;
    int getAvailableSeatsCount() const;

    // Utilities
    void displaySeats(SeatClass cls) const;

    static std::chrono::system_clock::time_point parseDateTime(const std::string& dt);
    static std::string formatDateTime(const std::chrono::system_clock::time_point& tp);
    static std::string formatTimeHM(const std::chrono::system_clock::time_point& tp);

    // Helper methods for serialization and serialization (data base dealing)
    json toJson() const;
    static Flight fromJson(const json& j);
};

#endif // FLIGHT_HPP