#ifndef ADMINISTRATOR_HPP
#define ADMINISTRATOR_HPP

#include "IUser.hpp"
#include "Passenger.hpp"
#include "BookingAgent.hpp"
#include "Pilot.hpp"
#include "FlightAttendent.hpp"

#include "FlightRepository.hpp"
#include "ReservationRepository.hpp"
#include "AircraftRepository.hpp"
#include "PassengerRepository.hpp"
#include "BookingAgentRepository.hpp"
#include "AdministratorRepository.hpp"
#include "MaintenanceRepository.hpp"

#include "Aircraft.hpp"
#include "Flight.hpp"
#include "MaintenanceLog.hpp"

#include "MaintenanceReport.hpp"
#include "OperationalReport.hpp"
#include "UserActivityReport.hpp"

#include <memory>
#include <string>
#include <vector>
#include <optional>


class Administrator : public IUser
{
private:
    // Repositories
    FlightRepository flightRepo;
    ReservationRepository reservationRepo;
    PassengerRepository passengerRepo;
    BookingAgentRepository agentRepo;
    AdministratorRepository adminRepo;
    AircraftRepository aircraftRepo;
    MaintenanceRepository maintenanceRepo;

public:
    // Constructor (new instance for signup) , pwd isn't hashed
    explicit Administrator(const std::string& uname, const std::string& plainPwd);
    // Constructor (old instance for fromJson), pwd is hashed
    explicit Administrator(const std::string& uname, const std::string& id , const std::string& hashedPwd);

    // Flight Management
    bool addFlight(const Flight& flight);
    bool updateFlight(const std::string& flightNum, const Flight& updatedFlight);
    bool removeFlight(const std::string& flightNum);
    std::vector<Flight> viewAllFlights() const;

    // Aircraft Management
    bool addAircraft(const Aircraft& aircraft);
    bool updateAircraft(const std::string& modelName, const Aircraft& updatedAircraft);
    bool removeAircraft(const std::string& modelName);
    std::vector<std::shared_ptr<Aircraft>> viewAllAircrafts() const;

    // Maintenance Logging
    bool logMaintenance(const std::string& aircraftModel, const std::string& description);
    std::vector<MaintenanceLog> viewAllMaintenanceLogs() const;

    // User Management
    bool addUser(const IUser& user);
    bool removeUser(const std::string& userId, Roles role);
    std::vector<std::shared_ptr<Passenger>> viewAllPassengers() const;
    std::vector<std::shared_ptr<BookingAgent>> viewAllBookingAgents() const;
    std::vector<std::shared_ptr<Administrator>> viewAllAdministrators() const;
    
    // Assing Crew
    bool assignCrewToFlight(std::shared_ptr<Flight> flight,
                                       std::shared_ptr<Pilot> pilot,
                                       std::shared_ptr<FlightAttendant> attendant);

    // Generating Reports
    void generateOperationalReport(const std::string& monthYear);
    void generateMaintenanceReport(const std::string& monthYear);
    void generateUserActivityReport(const std::string& monthYear);

    // overriding the pure viryual functions in IUser 
    void login() override;
    void logout() override;
    //void setId(const std::string& id) override;


};

#endif // ADMINISTRATOR_HPP