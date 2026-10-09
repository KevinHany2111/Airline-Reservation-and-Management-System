#ifndef MENU_HPP
#define MENU_HPP

#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Forward Declarations
class AuthenticationManager;
class Administrator;
class BookingAgent;
class Passenger;
class Flight;
class Seat;

class Menu
{
private:
    // Administrator submenu helpers
    static void admManageFlights(std::shared_ptr<Administrator> admin);
    static void admManageAircraft(std::shared_ptr<Administrator> admin);
    static void admManageUsers(std::shared_ptr<Administrator> admin);
    static void admGenerateReports(std::shared_ptr<Administrator> admin);

    static void admUpdateFlightMenu(std::shared_ptr<Administrator> admin);
    static void admUpdateFlightDetails(std::shared_ptr<Administrator> admin, const std::string& flightNumber);
    static void admAssignCrewToFlight(std::shared_ptr<Administrator> admin, const std::string& flightNumber);
    static void admUpdateFlightStatus(std::shared_ptr<Administrator> admin, const std::string& flightNumber);

    static void admLogMaintenance(std::shared_ptr<Administrator> admin);

    
    // BookingAgent submenu helpers
    static void agentSearchFlights(std::shared_ptr<BookingAgent> agent);
    static void agentBookFlight(std::shared_ptr<BookingAgent> agent);
    static void agentModifyReservation(std::shared_ptr<BookingAgent> agent);
    static void agentCancelReservation(const std::shared_ptr<BookingAgent>& agent);

    // Passenger submenu helpers
    static void passengerSearchFlights(std::shared_ptr<Passenger> passenger);
    static void passengerBookFlight(std::shared_ptr<Passenger> passenger);
    static void passengerViewReservations(std::shared_ptr<Passenger> passenger);
    static void passengerModifyReservation(std::shared_ptr<Passenger> passenger);
    static void passengerCancelReservation(std::shared_ptr<Passenger> passenger);
    static void passengerCheckIn(std::shared_ptr<Passenger> passenger);

    // Utilities for input
    static std::string promptLine(const std::string& prompt);
    static int promptInt(const std::string& prompt);

    // Serialization helpers (Menu builds json objects to write into repositories)
    static json buildFlightJsonInteractive();
    static json buildAircraftJsonInteractive();

public:

    // Main
    static int showMainMenu();
    static std::shared_ptr<class IUser> handleLogin(class AuthenticationManager& auth);
    
    // signUp needs passenger & agent repo paths to save new accounts
    static std::shared_ptr<class IUser> handleSignUp();

    // Role menus
    static void administratorMenu(std::shared_ptr<Administrator> admin);
    static void bookingAgentMenu(std::shared_ptr<BookingAgent> agent);
    static void passengerMenu(std::shared_ptr<Passenger> passenger);

};

#endif //MENU_HPP