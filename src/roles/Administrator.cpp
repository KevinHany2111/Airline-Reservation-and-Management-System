#include <iostream>
#include "Administrator.hpp"
#include "RepositoryUtils.hpp"
#include <RepositoryUtils.hpp>


// Constructor (new instance for signup) , pwd isn't hashed
Administrator::Administrator(const std::string& uname, const std::string& plainPwd) : 
                            IUser(uname, "A" + IdGenerator::generateId(), plainPwd, Roles::Administrator, IsPasswordHashed::no ),
                            flightRepo("data/flight.json"),
                            reservationRepo("data/reservations.json"),
                            passengerRepo("data/passengers.json"),
                            agentRepo("data/bookingAgents.json"),
                            adminRepo("data/administrators.json"),
                            aircraftRepo("data/aircrafts.json"),
                            maintenanceRepo("data/logs.json") {}
                            
// Constructor (old instance for fromJson), pwd is hashed
Administrator::Administrator(const std::string& uname, const std::string& id , const std::string& hashedPwd) : 
                            IUser(uname, id , hashedPwd, Roles::Administrator, IsPasswordHashed::yes),
                            flightRepo("data/flight.json"),
                            reservationRepo("data/reservations.json"),
                            passengerRepo("data/passengers.json"),
                            agentRepo("data/bookingAgents.json"),
                            adminRepo("data/administrators.json"),
                            aircraftRepo("data/aircrafts.json"),
                            maintenanceRepo("data/logs.json") {}

// Flight Management

bool Administrator::addFlight(const Flight& flight)
{
    return flightRepo.add(flight.toJson());
}


bool Administrator::updateFlight(const std::string& flightNum, const Flight& updatedFlight)
{
    auto existing = flightRepo.findById(flightNum);
    if(!existing) { return false; }
    return flightRepo.updateById(flightNum, updatedFlight.toJson());
}


bool Administrator::removeFlight(const std::string& flightNum)
{
    return flightRepo.removeById(flightNum);
}


std::vector<Flight> Administrator::viewAllFlights() const
{
    std::vector<Flight> flights;
    auto all = flightRepo.getAll();
    for(const auto& flight : all)
    {
        flights.push_back(Flight::fromJson(flight));
    }
    return flights;
}



// Aircraft Management
bool Administrator::addAircraft(const Aircraft& aircraft)
{
    return aircraftRepo.add(aircraft.toJson());
}

bool Administrator::updateAircraft(const std::string& modelName, const Aircraft& updatedAircraft)
{
    auto existing = aircraftRepo.findById(modelName);
    if(!existing) { return false; }
    return aircraftRepo.updateById(modelName, updatedAircraft.toJson());
}
bool Administrator::removeAircraft(const std::string& modelName)
{
    return aircraftRepo.removeById(modelName);
}

std::vector<std::shared_ptr<Aircraft>> Administrator::viewAllAircrafts() const
{
    std::vector<std::shared_ptr<Aircraft>> aircrafts;
    auto all = aircraftRepo.getAll();
    for (const auto& aircraftJson : all)
    {
        try {
            auto acPtr = Aircraft::fromJson(aircraftJson);
            aircrafts.push_back(acPtr);
        } catch (const std::exception& e) {
            // handle parsing error gracefully (log and skip)
            std::cerr << "[Administrator] Error parsing aircraft record: " << e.what() << std::endl;
        }
    }
    return aircrafts;
}

// Maintenance Logging

bool Administrator::logMaintenance(const std::string& aircraftModel, const std::string& description)
{
    MaintenanceLog log(aircraftModel, description); // engineer name auto generated
    return maintenanceRepo.add(log.toJson());
}

std::vector<MaintenanceLog> Administrator::viewAllMaintenanceLogs() const
{
    std::vector<MaintenanceLog> logs;
    auto all = maintenanceRepo.getAll();
    for (const auto& l : all)
        logs.push_back(MaintenanceLog::fromJson(l));
    return logs;
}

// User Management
bool Administrator::addUser(const IUser& user)
{
    switch(user.getRole())
    {
        case Roles::Passenger: 
            return passengerRepo.add(user.toJson());
        case Roles::BookingAgent:
            return agentRepo.add(user.toJson());
        case Roles::Administrator:
            return adminRepo.add(user.toJson());
        default:
            return false;
    }
}

bool Administrator::removeUser(const std::string& userId, Roles role)
{
    switch (role)
    {
    case Roles::Passenger:
        return passengerRepo.removeById(userId);
    case Roles::BookingAgent:
        return agentRepo.removeById(userId);
    case Roles::Administrator:
        return adminRepo.removeById(userId);
    default:
        return false;
    }
}

// Add in the Menu class showAllPassengers(std::vector<Passenger>) , agents , admins .. range-based loop -> cout each user info //you can merge the three functions into one showAll(std::vector<IUser>) sjows any type it accepts (casting) but for now make them three (because of the return type here)
std::vector<std::shared_ptr<Passenger>> Administrator::viewAllPassengers() const
{
    std::vector<std::shared_ptr<Passenger>> passengers;
    for (const auto& p : passengerRepo.getAll())
    {
        auto userPtr = IUser::fromJson(p); // Create correct derived type dynamically
        auto passPtr = std::dynamic_pointer_cast<Passenger>(userPtr);
        if (passPtr)
            passengers.push_back(passPtr);
    }
    return passengers;
}

std::vector<std::shared_ptr<BookingAgent>> Administrator::viewAllBookingAgents() const
{
    std::vector<std::shared_ptr<BookingAgent>> agents;
    for (const auto& a : agentRepo.getAll())
    {
        auto userPtr = IUser::fromJson(a);
        auto agentPtr = std::dynamic_pointer_cast<BookingAgent>(userPtr);
        if (agentPtr)
            agents.push_back(agentPtr);
    }
    return agents;
}

std::vector<std::shared_ptr<Administrator>> Administrator::viewAllAdministrators() const
{
    std::vector<std::shared_ptr<Administrator>> admins;
    for (const auto& ad : adminRepo.getAll())
    {
        auto userPtr = IUser::fromJson(ad);
        auto adminPtr = std::dynamic_pointer_cast<Administrator>(userPtr);
        if (adminPtr)
            admins.push_back(adminPtr);
    }
    return admins;
}

// Assinging Crew  
/*
bool Administrator::assignCrewToFlight(const std::string& flightNumber,
                                       const std::string& pilotId,
                                       const std::string& attendantId)
{
    auto flightJson = flightRepo.findById(flightNumber);
    if (!flightJson) { return false; }

    (*flightJson)["pilotId"] = pilotId;
    (*flightJson)["attendantId"] = attendantId;

    return flightRepo.updateById(flightNumber, *flightJson);
}
*/

bool Administrator::assignCrewToFlight(std::shared_ptr<Flight> flight,
                                       std::shared_ptr<Pilot> pilot,
                                       std::shared_ptr<FlightAttendant> attendant)
{
    if (!flight || !pilot || !attendant)
        return false;

    flight->assignPilot(pilot);
    flight->assignFlightAttendant(attendant);

    FlightRepository repo("data/flights.json");
    return repo.updateById(flight->getFlightNumber(), flight->toJson());
}


// Generating Reports

void Administrator::generateOperationalReport(const std::string& monthYear)
{
    OperationalReport report;
    report.generate(monthYear);
}

void Administrator::generateMaintenanceReport(const std::string& monthYear)
{
    MaintenanceReport report;
    report.generate(monthYear);
}

void Administrator::generateUserActivityReport(const std::string& monthYear)
{
    UserActivityReport report;
    report.generate(monthYear);
}


// overriding the pure viryual functions in IUser 
void Administrator::login()
{
    std::cout << "Logging in successfully, Administrator: " << getUserName() << std::endl;
}

void Administrator::logout()
{
    std::cout << "Logging out successfully, Administrator: " << getUserName() << std::endl;
}

//void Administrator::setId(const std::string& id) { userId = id; }