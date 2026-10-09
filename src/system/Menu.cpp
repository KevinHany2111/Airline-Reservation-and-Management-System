#include "Menu.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "system/AuthenticationManager.hpp"
#include "roles/Administrator.hpp"
#include "roles/BookingAgent.hpp"
#include "roles/Passenger.hpp"
#include "ICrewMember.hpp"
#include "Pilot.hpp"
#include "FlightAttendent.hpp"
#include "repositories/FlightRepository.hpp"
#include "repositories/PassengerRepository.hpp"
#include "repositories/BookingAgentRepository.hpp"
#include "repositories/AircraftRepository.hpp"
#include "core/Flight.hpp"
#include "core/Aircraft.hpp"
#include "utils/RepositoryUtils.hpp"
#include "utils/JsonHandler.hpp"
#include "utils/IdGenerator.hpp"
#include "system/ReservationService.hpp"
#include "core/Seat.hpp"
#include "payment/Cash.hpp"
#include "payment/CreditCard.hpp"
#include "payment/Paypal.hpp"

int Menu::showMainMenu()
{
    std::cout << "\nWelcome to Airline Reservation and Management System\n";
    std::cout << "1. Login\n";
    std::cout << "2. Sign Up\n";
    std::cout << "3. Exit\n";
    std::cout << "Choose option: ";
    int opt = 0;
    if (!(std::cin >> opt)) {
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        return 0;
    }
    std::cin.ignore(10000,'\n');
    return opt;
}

std::shared_ptr<class IUser> Menu::handleLogin(class AuthenticationManager& auth)
{
    std::cout << "\nLogin\n";
    std::cout << "Roles: 1=Administrator, 2=BookingAgent, 3=Passenger\n";
    int r = 0;
    bool valid = false;
    while(valid == false)
    {
        r = promptInt("Choose role: ");
        if(r <= 0 || r > 3)
        {
            std::cout << "Invalid Input, enter [1,2,3]" << std::endl;
        }
        else
        {
            valid = true;
        }
    }
    Roles role;
    switch(r)
    {
        case 1:   role = Roles::Administrator; break;
        case 2:   role = Roles::BookingAgent;  break;
        case 3:   role = Roles::Passenger;     break;
        default:  role = Roles::Passenger;     break;
    }
    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string uname = promptLine("Username: ");
    std::string pwd = promptLine("Password: ");

    auto user = auth.login(role, uname, pwd);
    return user; 
}

std::shared_ptr<class IUser> Menu::handleSignUp()
{
    std::cout << "\nSign Up\n";
    std::cout << "Enter role (1=Passenger, 2=BookingAgent): ";
    int r = 0;
    bool valid = false;
    while(valid == false)
    {
        std::cin >> r;
        if(r <= 0 || r > 2)
        {
            std::cout << "Invalid Input, enter [1,2]" << std::endl;
        }
        else
        {
            valid = true;
        }
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string uname = promptLine("Enter username: ");
    std::string pwd = promptLine("Enter password: ");
    if(r == 1)
    {
        auto passenger = std::make_shared<Passenger>(uname, pwd);
        json j = passenger->toJson();
        bool ok = RepositoryUtils::appendToFile("data/passengers.json", j);
        if (!ok)
        {
            std::cout << "Registration failed (couldn't write file).\n";
            return nullptr;
        }
        std::cout << "Registration successful! Welcome, " << uname << "\n";
        return passenger;
    }
    else if(r == 2)
    {
        auto agent = std::make_shared<BookingAgent>(uname, pwd);
        json j = agent->toJson();
        bool ok = RepositoryUtils::appendToFile("data/bookingAgents.json", j);
        if (!ok)
        {
            std::cout << "Registration failed (couldn't write file).\n";
            return nullptr;
        }
        std::cout << "Registration successful! Welcome, " << uname << "\n";
        return agent;
    }
    else
    {
        std::cout << "Invalid role choice.\n";
        return nullptr;
    }

}


// ----------------- ADMINISTRATOR MENU -----------------

void Menu::administratorMenu(std::shared_ptr<Administrator> admin)
{
    if (!admin) { std::cout << "[Error] Null administrator reference.\n"; return; }
    while(true)
    {
        std::cout << "\n--- Administrator Menu ---\n"
                  << "1. Manage Flights\n"
                  << "2. Manage Aircraft\n"
                  << "3. Manage Users\n"
                  << "4. Generate Reports\n"
                  << "5. Logout\n"
                  << "Enter choice: ";
        
        bool valid = false;
        int c = 0;
        while(valid == false)
        {
            if (!(std::cin >> c)) { std::cin.clear(); std::cin.ignore(10000,'\n'); continue; }
            std::cin.ignore(10000,'\n');
            if(c <=0 || c > 5)
            {
                std::cout << "Invalid choice." << std::endl;
            }
            else
            {
                valid = true;
            }
        }

        if (c == 1) admManageFlights(admin);
        else if (c == 2) admManageAircraft(admin);
        else if (c == 3) admManageUsers(admin);
        else if (c == 4) admGenerateReports(admin);
        else if (c == 5) { break; }
        else
        {
            std::cout << "Invalid Choice.." << std::endl;
        }
    }
}



void Menu::admManageFlights(std::shared_ptr<Administrator> admin)
{
    while (true)
    {
        std::cout << "\n--- Manage Flights ---\n"
                  << "1. Add New Flight\n"
                  << "2. Update Existing Flight\n"
                  << "3. Remove Flight\n"
                  << "4. View All Flights\n"
                  << "5. Back to Main Menu\n"
                  << "Enter choice: ";

        int c = promptInt("");

        if (c == 1)
        {
            json j = buildFlightJsonInteractive();
            FlightRepository repo("data/flights.json");
            if (repo.add(j))
                std::cout << "Flight added successfully.\n";
            else
                std::cout << "Failed to add flight.\n";
        }
        else if (c == 2)
        {
            admUpdateFlightMenu(admin);
        }
        else if (c == 3)
        {
            std::string fn = promptLine("Enter Flight Number to remove: ");
            FlightRepository repo("data/flights.json");
            if (repo.removeById(fn))
                std::cout << "Flight removed.\n";
            else
                std::cout << "Failed to remove flight.\n";
        }
        else if (c == 4)
        {
            FlightRepository repo("data/flights.json");
            auto all = repo.getAll();
            std::cout << "\nAll Flights:\n";
            for (const auto &f : all)
            {
                try
                {
                    auto fl = Flight::fromJson(f);
                    std::cout << fl.getFlightNumber() << " | " << fl.getOrigin()
                              << " -> " << fl.getDestination()
                              << " | Departure: " << fl.getDepartureDateTimeString()
                              << " | Status: " << fl.getStatusStr() << "\n";
                }
                catch (...)
                {
                    std::cout << f.dump() << "\n";
                }
            }
        }
        else if (c == 5)
            break;
        else
            std::cout << "Invalid choice.\n";
    }
}


/*
void Menu::admManageFlights(std::shared_ptr<Administrator> admin)
{
    while (true)
    {
        std::cout << "\n--- Manage Flights ---\n"
                  << "1. Add New Flight\n"
                  << "2. Update Scheduled Flight\n"
                  << "3. Finish Arrived Flight\n"
                  << "4. Remove Flight\n"
                  << "5. View All Flights\n"
                  << "6. Back to Main Menu\n"
                  << "Enter choice: ";
        int c = promptInt("");
        if (c == 1)
        {
            json j = buildFlightJsonInteractive();
            FlightRepository repo("data/flights.json");
            if (repo.add(j)) std::cout << "Flight added successfully.\n";
            else std::cout << "Failed to add flight.\n";
        }
        else if (c == 2)
        {
            std::string fn = promptLine("Enter Flight Number to update: ");
            json updated = buildFlightJsonInteractive();
            FlightRepository repo("data/flights.json");
            if (repo.updateById(fn, updated)) std::cout << "Flight updated.\n";
            else std::cout << "Failed to update flight (not found?).\n";
        }
        else if(c == 3)
        {
            std::string fn = promptLine("Enter Arrived Flight Number : ");
            FlightRepository repo("data/flights.json");
            json updated = repo.findById(fn);
            updated["status"] = "Completed";
            if (repo.updateById(fn, updated)) std::cout << "Flight updated.\n";
            else std::cout << "Failed to update flight (not found?).\n";
        }
        else if (c == 4)
        {
            std::string fn = promptLine("Enter Flight Number to remove: ");
            FlightRepository repo("data/flights.json");
            if (repo.removeById(fn)) std::cout << "Flight removed.\n";
            else std::cout << "Failed to remove flight.\n";
        }
        else if (c == 5)
        {
            FlightRepository repo("data/flights.json");
            auto all = repo.getAll();
            std::cout << "\nAll Flights:\n";
            for (const auto &f : all)
            {
                // try to convert to Flight if available
                try {
                    auto fl = Flight::fromJson(f);
                    std::cout << fl.getFlightNumber() << " | " << fl.getOrigin()
                              << " -> " << fl.getDestination()
                              << " | " << " Departure: " << fl.getDepartureDateTimeString() << "\n";
                } catch (...) {
                    std::cout << f.dump() << "\n";
                }
            }
        }
        else if (c == 6) break;
        else std::cout << "Invalid choice.\n";
    }
}
*/


void Menu::admManageAircraft(std::shared_ptr<Administrator> admin)
{
    while (true)
    {
        std::cout << "\n--- Manage Aircraft ---\n"
                  << "1. Add Aircraft\n"
                  << "2. Update Aircraft\n"
                  << "3. Remove Aircraft\n"
                  << "4. View All Aircrafts\n"
                  << "5. Log Maintenance\n"
                  << "6. Back to Admin Menu\n"
                  << "Enter choice: ";

        int c = promptInt("");
        AircraftRepository arepo("data/aircrafts.json");

        if (c == 1)
        {
            json j = buildAircraftJsonInteractive();
            if (arepo.add(j))
                std::cout << "Aircraft added.\n";
            else
                std::cout << "Failed to add aircraft.\n";
        }
        else if (c == 2)
        {
            std::string model = promptLine("Enter model name to update: ");
            json j = buildAircraftJsonInteractive();
            if (arepo.updateById(model, j))
                std::cout << "Updated successfully.\n";
            else
                std::cout << "Failed to update.\n";
        }
        else if (c == 3)
        {
            std::string model = promptLine("Enter model name to remove: ");
            if (arepo.removeById(model))
                std::cout << "Aircraft removed.\n";
            else
                std::cout << "Failed to remove aircraft.\n";
        }
        else if (c == 4)
        {
            auto all = arepo.getAll();
            for (const auto &a : all)
            {
                try
                {
                    auto ac = Aircraft::fromJson(a); // shared_ptr<Aircraft>
                    if (ac)
                        std::cout << ac->getModelName() << " — Seats: "
                                  << a.value("totalSeats", 0) << "\n";
                    else
                        std::cout << "[Menu] Failed to load aircraft record\n";
                }
                catch (const std::exception &e)
                {
                    std::cout << a.dump() << "\n";
                }
            }
        }
        else if (c == 5)
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::string aircraftName = promptLine("Enter Aircraft Model Name: ");
            std::string description = promptLine("Enter Maintenance Description: ");

            if (aircraftName.empty() || description.empty())
            {
                std::cout << "Error: Aircraft name and description cannot be empty.\n";
                continue;
            }

            MaintenanceLog log(aircraftName, description);
            MaintenanceRepository mrepo("data/MaintenanceLogs.json");

            if (mrepo.add(log.toJson()))
            {
                std::cout << "   Maintenance logged successfully.\n"
                          << "   Engineer: " << log.getEngineerName() << "\n"
                          << "   Log ID: " << log.getLogId() << "\n";
            }
            else
            {
                std::cout << "Failed to log maintenance.\n";
            }
        }
        else if (c == 6)
        {
            std::cout << "Returning to Admin Menu...\n";
            break;
        }
        else
        {
            std::cout << "Invalid option. Try again.\n";
        }
    }
}


/*
void Menu::admManageAircraft(std::shared_ptr<Administrator> admin)
{
    while (true)
    {
        std::cout << "\n--- Manage Aircraft ---\n"
                  << "1. Add Aircraft\n"
                  << "2. Update Aircraft\n"
                  << "3. Remove Aircraft\n"
                  << "4. View All Aircrafts\n"
                  << "5. Back to Admin Menu\n"
                  << "Enter choice: ";
        int c = promptInt("");
        AircraftRepository arepo("data/aircrafts.json");
        if (c == 1)
        {
            json j = buildAircraftJsonInteractive();
            if (arepo.add(j)) std::cout << "Aircraft added.\n";
            else std::cout << "Failed to add aircraft.\n";
        }
        else if (c == 2)
        {
            std::string model = promptLine("Enter model name to update: ");
            json j = buildAircraftJsonInteractive();
            if (arepo.updateById(model, j)) std::cout << "Updated.\n";
            else std::cout << "Failed to update.\n";
        }
        else if (c == 3)
        {
            std::string model = promptLine("Enter model name to remove: ");
            if (arepo.removeById(model)) std::cout << "Removed.\n";
            else std::cout << "Failed to remove.\n";
        }
        else if (c == 4)
        {
            auto all = arepo.getAll();
            for (const auto &a : all) 
            {
                try 
                {
                    auto ac = Aircraft::fromJson(a); // now shared_ptr<Aircraft>
                    if (ac) {
                        std::cout << ac->getModelName() << " seats: " << a.value("totalSeats", 0) << "\n";
                    } else {
                        std::cout << "[Menu] Failed to load aircraft record\n";
                    }
                } 
                catch (const std::exception& e) 
                {
                    std::cout << a.dump() << "\n";
                }
            }
        }
        else if (c == 5) break;
        else std::cout << "Invalid option.\n";
    }
}
*/


void Menu::admLogMaintenance(std::shared_ptr<Administrator> admin)
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string aircraftName, description;

    std::cout << "\nEnter Aircraft Model Name: ";
    std::getline(std::cin, aircraftName);

    std::cout << "Enter Maintenance Description: ";
    std::getline(std::cin, description);

    if (aircraftName.empty() || description.empty())
    {
        std::cout << "Error: Aircraft name and description cannot be empty.\n";
        return;
    }

    MaintenanceLog log(aircraftName, description);

    MaintenanceRepository repo("data/maintenanceLogs.json");
    if (repo.add(log.toJson()))
        std::cout << "Maintenance logged successfully by " << log.getEngineerName()
                  << " (Log ID: " << log.getLogId() << ")\n";
    else
        std::cout << "Failed to log maintenance.\n";
}



void Menu::admManageUsers(std::shared_ptr<Administrator> admin)
{
    while (true)
    {
        std::cout << "\n--- Manage Users ---\n"
                  << "1. View All Passengers\n"
                  << "2. View All Booking Agents\n"
                  << "3. View All Administrators\n"
                  << "4. Add User (Passenger/Agent)\n"
                  << "5. Remove User by ID\n"
                  << "6. Back\n"
                  << "Enter choice: ";
        int c = promptInt("");
        PassengerRepository prepo("data/users/passengers.json");
        BookingAgentRepository arepo("data/users/bookingAgents.json");
        AdministratorRepository adminRepo("data/users/administrators.json");

        if (c == 1)
        {
            auto all = prepo.getAll();
            std::cout << "\nPassengers:\n";
            for (const auto &p : all) {
                std::cout << p.value("id","-") << " | " << p.value("username","-") << "\n";
            }
        }
        else if (c == 2)
        {
            auto all = arepo.getAll();
            std::cout << "\nBooking Agents:\n";
            for (const auto &p : all) {
                std::cout << p.value("id","-") << " | " << p.value("username","-") << "\n";
            }
        }
        else if (c == 3)
        {
            auto all = adminRepo.getAll();
            std::cout << "\nAdministrators:\n";
            for (const auto &p : all) {
                std::cout << p.value("id","-") << " | " << p.value("username","-") << "\n";
            }
        }
        else if (c == 4)
        {
            int r = promptInt("Role (1=Passenger, 2=BookingAgent): ");
            std::string uname = promptLine("Username: ");
            std::string pwd = promptLine("Password: ");
            if (r == 1) {
                auto passenger = std::make_shared<Passenger>(uname,pwd);
                json j = passenger->toJson();
                if (prepo.add(j)) std::cout << "Passenger added.\n"; else std::cout << "Failed.\n";
            } else {
                auto agent = std::make_shared<BookingAgent>(uname,pwd);
                json j = agent->toJson();
                if (arepo.add(j)) std::cout << "BookingAgent added.\n"; else std::cout << "Failed.\n";
            }
        }
        else if (c == 5)
        {
            std::string id = promptLine("Enter user id to remove: ");
            // try all repos
            if (prepo.removeById(id) || arepo.removeById(id) || adminRepo.removeById(id))
                std::cout << "Removed if existed.\n";
            else std::cout << "No such id found.\n";
        }
        else if (c == 6) break;
        else std::cout << "Invalid option.\n";
    }
}

void Menu::admGenerateReports(std::shared_ptr<Administrator> admin)
{
    while(true)
    {
        std::cout << "\n--- Generate Reports ---\n"
                << "1. Operational Reports\n"
                << "2. Maintenance Reports\n"
                << "3. User Activity Reports\n"
                << "4. Back\n"
                << "Enter choice: ";
        int c = promptInt("");
        if (c == 1)
        {
            std::string month = promptLine("Enter Month and Year (MM-YYYY): ");
            admin->generateOperationalReport(month);
        }
        else if (c == 2)
        {
            std::string month = promptLine("Enter Month and Year (MM-YYYY): ");
            admin->generateMaintenanceReport(month);
        }
        else if (c == 3)
        {
            std::string month = promptLine("Enter Month and Year (MM-YYYY): ");
            admin->generateUserActivityReport(month);
        }
        else if(c == 4)
        {
            std::cout << "Back.\n";
            break;
        }
        else
        {
            std::cout << "Invalid option.\n";
        }
    }
}


void Menu::admUpdateFlightMenu(std::shared_ptr<Administrator> admin)
{
    std::string flightNum = promptLine("Enter Flight Number to update: ");

    while (true)
    {
        std::cout << "\n--- Update Existing Flight ---\n"
                  << "Select information to update:\n"
                  << "1. Flight Details\n"
                  << "2. Crew Assignments\n"
                  << "3. Status\n"
                  << "4. Back to Manage Flights\n"
                  << "Enter choice: ";

        int c = promptInt("");

        if (c == 1)
        {
            admUpdateFlightDetails(admin, flightNum);
        }
        else if (c == 2)
        {
            admAssignCrewToFlight(admin, flightNum);
        }
        else if (c == 3)
        {
            admUpdateFlightStatus(admin, flightNum);
        }
        else if (c == 4)
            break;
        else
            std::cout << "Invalid choice.\n";
    }
}

void Menu::admUpdateFlightDetails(std::shared_ptr<Administrator> admin, const std::string& flightNumber)
{
    json updated = buildFlightJsonInteractive();
    FlightRepository repo("data/flights.json");

    if (repo.updateById(flightNumber, updated))
        std::cout << "Flight details updated successfully.\n";
    else
        std::cout << "Failed to update flight details (not found?).\n";
}
void Menu::admAssignCrewToFlight(std::shared_ptr<Administrator> admin, const std::string& flightNumber)
{
    // --- Load the flight from repository ---
    FlightRepository frepo("data/flights.json");
    auto fjson = frepo.findById(flightNumber);

    if (!fjson)
    {
        std::cout << "Flight not found.\n";
        return;
    }

    // Deserialize existing flight (with aircraft, seats, status, etc.)
    auto flight = std::make_shared<Flight>(Flight::fromJson(*fjson));

    std::cout << "\n--- Crew Assignments ---\n";

    // --- Show current crew info (if any) ---
    if (flight->getPilot())
    {
        std::cout << "Current Pilot: " << flight->getPilot()->getName()
                  << " (ID: " << flight->getPilot()->getId() << ")\n";
    }
    if (flight->getFlightAttendant())
    {
        std::cout << "Current Flight Attendant: " << flight->getFlightAttendant()->getName()
                  << " (ID: " << flight->getFlightAttendant()->getId() << ")\n";
    }

    // --- Available Pilots (simulate existing system list) ---
    std::vector<std::shared_ptr<Pilot>> pilots = {
        std::make_shared<Pilot>("Captain Sarah Connor"),
        std::make_shared<Pilot>("Captain James Kirk")
    };
    pilots[0]->ICrewMember::id = "PL001";
    pilots[1]->ICrewMember::id = "PL002";

    std::cout << "\nAvailable Pilots:\n";
    for (size_t i = 0; i < pilots.size(); ++i)
    {
        std::cout << i + 1 << ". Pilot ID: " << pilots[i]->getId()
                  << " - " << pilots[i]->getName() << "\n";
    }

    // --- Select Pilot ---
    std::string selectedPilotId = promptLine("\nSelect Pilot by ID: ");
    std::shared_ptr<Pilot> selectedPilot = nullptr;
    for (const auto& p : pilots)
    {
        if (p->getId() == selectedPilotId)
        {
            selectedPilot = p;
            break;
        }
    }

    if (!selectedPilot)
    {
        std::cout << "Invalid Pilot ID.\n";
        return;
    }

    // --- Available Flight Attendants (simulate existing system list) ---
    std::vector<std::shared_ptr<FlightAttendant>> attendants = {
        std::make_shared<FlightAttendant>("Emma Watson"),
        std::make_shared<FlightAttendant>("John Cena")
    };
    attendants[0]->ICrewMember::id = "FA001";
    attendants[1]->ICrewMember::id = "FA002";

    std::cout << "\nAvailable Flight Attendants:\n";
    for (size_t i = 0; i < attendants.size(); ++i)
    {
        std::cout << i + 1 << ". " << attendants[i]->getId()
                  << " - " << attendants[i]->getName() << "\n";
    }

    // --- Select Attendant ---
    std::string selectedAttendantId = promptLine("\nSelect Flight Attendant by ID: ");
    std::shared_ptr<FlightAttendant> selectedAttendant = nullptr;
    for (const auto& a : attendants)
    {
        if (a->getId() == selectedAttendantId)
        {
            selectedAttendant = a;
            break;
        }
    }

    if (!selectedAttendant)
    {
        std::cout << "Invalid Flight Attendant ID.\n";
        return;
    }

    // --- Assign Crew ---
    flight->assignPilot(selectedPilot);
    flight->assignFlightAttendant(selectedAttendant);

    // --- Persist changes ---
    bool updated = frepo.updateById(flightNumber, flight->toJson());
    if (updated)
    {
        std::cout << "\nCrew assigned successfully to Flight "
                  << flightNumber << ".\n";
    }
    else
    {
        std::cout << "\nError: Failed to update flight.\n";
    }
}


void Menu::admUpdateFlightStatus(std::shared_ptr<Administrator> admin, const std::string& flightNumber)
{
    FlightRepository repo("data/flights.json");
    auto flightJson = repo.findById(flightNumber);

    if (!flightJson)
    {
        std::cout << "Flight not found.\n";
        return;
    }

    std::cout << "\nSelect new status:\n"
              << "1. Scheduled\n"
              << "2. Delayed\n"
              << "3. Cancelled\n"
              << "4. Completed\n"
              << "5. Back\n";

    int c;
    std::string newStatus;

    while (true)
    {
        std::cout << "Enter choice: ";
        c = promptInt("");

        if (c == 5)
            return; // go back

        if (c == 1) { newStatus = "Scheduled"; break; }
        if (c == 2) { newStatus = "Delayed"; break; }
        if (c == 3) { newStatus = "Cancelled"; break; }
        if (c == 4) { newStatus = "Completed"; break; }

        std::cout << "Invalid choice. Please try again.\n";
    }

    // only executed after valid input
    (*flightJson)["status"] = newStatus;
    if (repo.updateById(flightNumber, *flightJson))
        std::cout << "Flight status updated to " << newStatus << ".\n";
    else
        std::cout << "Failed to update status.\n";
}



// ----------------- BOOKING AGENT MENU -----------------

void Menu::bookingAgentMenu(std::shared_ptr<BookingAgent> agent)
{
    if (!agent) { std::cout << "[Error] Null agent reference.\n"; return; }

    while (true)
    {
        std::cout << "\n--- Booking Agent Menu ---\n"
                  << "1. Search Flights\n"
                  << "2. Book a Flight\n"
                  << "3. Modify Reservation\n"
                  << "4. Cancel Reservation\n"
                  << "5. Logout\n"
                  << "Enter choice: ";

        int c = promptInt("");
        switch (c)
        {
        case 1: agentSearchFlights(agent); break;
        case 2: agentBookFlight(agent); break;
        case 3: agentModifyReservation(agent); break;
        case 4: agentCancelReservation(agent); break;
        case 5:
            std::cout << "Logging out booking agent...\n";
            return;
        default:
            std::cout << "Invalid option.\n";
        }
    }
}


// ========== 1️⃣ Search Flights (with optional booking) ==========
void Menu::agentSearchFlights(std::shared_ptr<BookingAgent> agent)
{
    if (!agent)
    {
        std::cout << "[Error] Null BookingAgent reference.\n";
        return;
    }

    std::string origin = promptLine("Enter Origin: ");
    std::string destination = promptLine("Enter Destination: ");
    std::string departureDate = promptLine("Enter Departure Date (YYYY-MM-DD): ");

    FlightRepository repo("data/flights.json");
    auto flights = repo.findFlights(origin, destination, departureDate);

    agent->searchFlights(flights);

    if (flights.empty())
        return;

    std::string choice = promptLine("Do you want to book a flight for a passenger [y/n]: ");
    while (true)
    {
        if (choice == "y" || choice == "Y")
        {
            agentBookFlight(agent);
            break;
        }
        else if (choice == "n" || choice == "N")
        {
            break;
        }
        else
        {
            choice = promptLine("Invalid input, please enter [y/n]: ");
        }
    }
}


// ========== 2️⃣ Book Flight for Passenger ==========
void Menu::agentBookFlight(std::shared_ptr<BookingAgent> agent)
{
    if (!agent)
    {
        std::cout << "[Error] Invalid booking agent.\n";
        return;
    }

    // ----------------- Load Passenger -----------------
    std::string passengerId = promptLine("Enter Passenger ID: ");
    PassengerRepository passengerRepo("data/passengers.json");
    auto pjson = passengerRepo.findById(passengerId);
    if (!pjson)
    {
        std::cout << "Error: Passenger not found.\n";
        return;
    }

    std::shared_ptr<Passenger> passenger;
    try
    {
        passenger = std::dynamic_pointer_cast<Passenger>(IUser::fromJson(*pjson));
        if (!passenger)
        {
            std::cout << "Error: Failed to cast IUser to Passenger.\n";
            return;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Error constructing passenger: " << e.what() << std::endl;
        return;
    }

    // ----------------- Load Flight -----------------
    std::string flightNumber = promptLine("Enter Flight Number: ");
    FlightRepository frepo("data/flights.json");
    auto fjson = frepo.findById(flightNumber);
    if (!fjson)
    {
        std::cout << "Error: Flight not found.\n";
        return;
    }

    auto flight = std::make_shared<Flight>(Flight::fromJson(*fjson));
    auto aircraft = flight->getAircraft();
    if (!aircraft)
    {
        std::cout << "Error: Flight has no aircraft assigned.\n";
        return;
    }

    if (flight->getStatus() == FlightStatus::Cancelled)
    {
        std::cout << "This flight is cancelled. Booking not allowed.\n";
        return;
    }

    if (flight->getStatus() == FlightStatus::Completed)
    {
        std::cout << "This flight has already been completed.\n";
        return;
    }

    // ----------------- Seat Selection -----------------
    std::shared_ptr<Seat> chosenSeat = nullptr;

    while (true)
    {
        std::string seatClass = promptLine("Enter Seat Class [First/Business/Economy]: ");
        SeatClass selectedClass;

        if (seatClass == "First") selectedClass = SeatClass::First;
        else if (seatClass == "Business") selectedClass = SeatClass::Business;
        else if (seatClass == "Economy") selectedClass = SeatClass::Economy;
        else { std::cout << "Invalid class.\n"; continue; }

        std::cout << "\nAvailable seats in " << seatClass << " class:\n\n";
        flight->displaySeats(selectedClass);
        std::cout << std::endl;

        std::string seatId = promptLine("Enter Seat Number (e.g., 12A) (0 to cancel): ");
        if (seatId == "0") return;

        auto seat = aircraft->getSeatById(seatId);
        if (!seat)
        {
            std::cout << "Invalid seat ID.\n";
            continue;
        }

        if (seat->getSeatClass() != selectedClass)
        {
            std::cout << "Seat " << seatId << " not in selected class.\n";
            continue;
        }

        if (!seat->isAvailableSeat())
        {
            std::cout << "Seat already occupied.\n";
            continue;
        }

        chosenSeat = seat;
        break;
    }

    // ----------------- Payment -----------------
    std::string method = promptLine("Enter Payment Method (CreditCard/Cash/PayPal): ");
    std::shared_ptr<IPayment> payment = nullptr;

    if (method == "Cash" || method == "cash")
        payment = std::make_shared<Cash>(chosenSeat->getSeatPrice());
    else if (method == "CreditCard" || method == "creditcard")
    {
        std::string card = promptLine("Enter card number: ");
        payment = std::make_shared<CreditCard>(chosenSeat->getSeatPrice(), card);
    }
    else if (method == "PayPal" || method == "paypal")
    {
        std::string email = promptLine("Enter PayPal email: ");
        payment = std::make_shared<Paypal>(chosenSeat->getSeatPrice(), email);
    }
    else
    {
        std::cout << "Unknown payment method.\n";
        return;
    }

    // ----------------- Reservation -----------------
    ReservationService service;
    auto reservation = service.reserve(passenger, flight, chosenSeat, payment, std::nullopt);

    if (!reservation)
    {
        std::cout << "Reservation failed.\n";
        return;
    }

    // Update flight file (seat taken)
    chosenSeat->setAvailability(false);
    frepo.updateById(flight->getFlightNumber(), flight->toJson());

    std::cout << "\nBooking completed successfully!\n";
}


// ========== 3️⃣ Modify Reservation ==========
void Menu::agentModifyReservation(std::shared_ptr<BookingAgent> agent)
{
    if (!agent)
    {
        std::cout << "[Error] Invalid booking agent.\n";
        return;
    }

    std::string reservationId = promptLine("Enter Reservation ID to modify: ");
    ReservationRepository resRepo("data/reservations.json");
    auto rjson = resRepo.findById(reservationId);

    if (!rjson)
    {
        std::cout << "Error: Reservation not found.\n";
        return;
    }

    auto reservation = std::make_shared<Reservation>(Reservation::fromJson(*rjson));
    auto flight = reservation->getFlight();

    if (!flight)
    {
        FlightRepository frepo("data/flights.json");
        auto fjson = frepo.findById(rjson->at("flightNumber"));
        if (!fjson)
        {
            std::cout << "Error: Related flight not found.\n";
            return;
        }
        flight = std::make_shared<Flight>(Flight::fromJson(*fjson));
    }

    auto aircraft = flight->getAircraft();
    if (!aircraft)
    {
        std::cout << "Error: Flight has no aircraft.\n";
        return;
    }

    auto oldSeat = aircraft->getSeatById(rjson->at("seatId"));
    SeatClass seatClass = oldSeat ? oldSeat->getSeatClass() : SeatClass::Economy;

    std::cout << "\n--- Modify Reservation ---\n";
    std::cout << "Available seats in " << Seat::seatClassToString(seatClass) << " class:\n\n";
    flight->displaySeats(seatClass);

    std::shared_ptr<Seat> newSeat = nullptr;

    while (true)
    {
        std::string newSeatId = promptLine("Enter new seat number (e.g., 12A) (0 to cancel): ");
        if (newSeatId == "0") return;

        newSeat = aircraft->getSeatById(newSeatId);
        if (!newSeat)
        {
            std::cout << "Invalid seat ID.\n";
            continue;
        }
        if (newSeat->getSeatClass() != seatClass)
        {
            std::cout << "Seat not in same class.\n";
            continue;
        }
        if (!newSeat->isAvailableSeat())
        {
            std::cout << "Seat already occupied.\n";
            continue;
        }
        break;
    }

    
    if(!agent)
    {
        std::cout << "[debug] actor null ptr" << std::endl;
    }
    if(!reservation)
    {
        std::cout << "[debug] reservation null ptr" << std::endl;
    }
    if(!newSeat)
    {
        std::cout << "[debug] newSeat null ptr" << std::endl;
    }
    int x;
    std::cin >> x ;

    ReservationService service;
    if (service.modifyReservation(agent, reservation, newSeat))
        std::cout << "Reservation modified successfully.\n";
    else
        std::cout << "Modification failed.\n";
}


// ========== 4️⃣ Cancel Reservation ==========

void Menu::agentCancelReservation(const std::shared_ptr<BookingAgent>& agent)
{
    std::string reservationId = promptLine("Enter reservation ID to cancel: ");

    // ✅ Load all reservations using RepositoryUtils
    auto reservationsJson = RepositoryUtils::getAll("data/reservations.json");
    if (reservationsJson.empty()) {
        std::cout << "No reservations found.\n";
        return;
    }

    // ✅ Find reservation by ID in JSON
    auto it = std::find_if(reservationsJson.begin(), reservationsJson.end(),
        [&](const json& j) {
            return j.value("reservationId", "") == reservationId;
        });

    if (it == reservationsJson.end()) {
        std::cout << "Reservation not found.\n";
        return;
    }

    const json& reservationJson = *it;

    // ✅ Load passenger info from file
    std::string passengerId = reservationJson.value("passengerId", "");
    auto passengerJsonOpt = RepositoryUtils::getById(
        "data/passengers.json", "passengerId", passengerId
    );

    if (!passengerJsonOpt.has_value()) {
        std::cout << "Passenger not found for this reservation.\n";
        return;
    }

    const auto& passengerJson = passengerJsonOpt.value();

    // ✅ Display reservation info directly from JSON
    std::cout << "\n--- Reservation Details ---\n";
    std::cout << "Reservation ID: " << reservationJson.value("reservationId", "N/A") << "\n";
    std::cout << "Flight Number: " << reservationJson.value("flightNumber", "N/A") << "\n";
    std::cout << "Seat: " << reservationJson.value("seatId", "N/A") << "\n";
    std::cout << "Cancelled: " << (reservationJson.value("cancelled", false) ? "Yes" : "No") << "\n";

    // ✅ Display passenger info
    std::cout << "\n--- Passenger Details ---\n";
    std::cout << "Passenger ID: " << passengerJson.value("passengerId", "N/A") << "\n";
    std::cout << "Name: " << passengerJson.value("name", "N/A") << "\n";
    std::cout << "Email: " << passengerJson.value("email", "N/A") << "\n";

    std::string confirm;
    std::cout << "\nConfirm cancellation? (y/n): ";
    std::getline(std::cin >> std::ws, confirm);

    if (confirm != "y" && confirm != "Y") {
        std::cout << "Cancellation aborted.\n";
        return;
    }

    // ✅ Convert JSON reservation to object before passing to service
    Reservation reservation = Reservation::fromJson(reservationJson);
    auto reservationPtr = std::make_shared<Reservation>(reservation);

    // ✅ Safely call the ReservationService to handle seat + file updates
    ReservationService service;
    bool success = service.cancelReservation(
        std::static_pointer_cast<IUser>(agent),
        reservationPtr
    );

    if (success)
        std::cout << "Reservation cancelled successfully.\n";
    else
        std::cout << "Failed to cancel reservation.\n";
}





/*
// ----------------- BOOKING AGENT MENU -----------------

void Menu::bookingAgentMenu(std::shared_ptr<BookingAgent> agent)
{
    if (!agent) { std::cout << "[Error] Null agent reference.\n"; return; }
    while (true)
    {
        std::cout << "\n--- Booking Agent Menu ---\n"
                  << "1. Search Flights\n"
                  << "2. Book a Flight\n"
                  << "3. Modify Reservation\n"
                  << "4. Cancel Reservation\n"
                  << "5. Logout\n"
                  << "Enter choice: ";
        int c = promptInt("");
        if (c == 1) agentSearchFlights(agent);
        else if (c == 2) agentBookFlight(agent);
        else if (c == 3) agentModifyReservation(agent);
        else if (c == 4) agentCancelReservation(agent);
        else if (c == 5) { std::cout << "Logging out booking agent...\n"; break; }
        else std::cout << "Invalid option.\n";
    }
}

void Menu::agentSearchFlights(std::shared_ptr<BookingAgent> agent)
{
    if (!agent)
    {
        std::cout << "[Error] Null BookingAgent reference.\n";
        return;
    }

    std::string origin = promptLine("Enter Origin: ");
    std::string destination = promptLine("Enter Destination: ");
    std::string departureDate = promptLine("Enter Departure Date (YYYY-MM-DD): ");

    // Use repository logic (includes next day results automatically)
    FlightRepository repo("data/flights.json");
    auto flights = repo.findFlights(origin, destination, departureDate);

    // Delegate printing to BookingAgent’s method
    agent->searchFlights(flights);
} 


void Menu::agentBookFlight(std::shared_ptr<BookingAgent> agent)
{
    if (!agent)
    {
        std::cout << "[Error] Invalid booking agent.\n";
        return;
    }

    std::string passengerId = promptLine("Enter Passenger ID: ");
    std::string flightNumber = promptLine("Enter Flight Number: ");
    std::string seatId = promptLine("Enter Seat Number (e.g., 12A): ");

    // --- Fetch Passenger (as IUser first) ---
    PassengerRepository passengerRepo("data/users/passengers.json");
    auto pjson = passengerRepo.findById(passengerId);
    if (!pjson)
    {
        std::cout << "Error: Passenger not found.\n";
        return;
    }

    // If repository returns IUser-type, downcast safely:
    std::shared_ptr<Passenger> passenger;
    try
    {
        auto passenger = Passenger::fromJson(*pjson, "userId");
    }
    catch (const std::exception& e)
    {
        std::cout << "Error constructing passenger: " << e.what() << std::endl;
        return;
    }

    if (!passenger)
    {
        std::cout << "Error: Could not load passenger data.\n";
        return;
    }

    // --- Fetch Flight ---
    FlightRepository flightRepo("data/flights.json");
    auto fjson = flightRepo.findById(flightNumber);
    if (!fjson)
    {
        std::cout << "Error: Flight not found.\n";
        return;
    }

    auto flight = std::make_shared<Flight>(Flight::fromJson(*fjson));

    // --- Seat ---
    auto aircraft = flight->getAircraft();
    if (!aircraft)
    {
        std::cout << "Error: Flight has no aircraft assigned.\n";
        return;
    }

    auto seat = aircraft->getSeatById(seatId);
    if (!seat)
    {
        std::cout << "Error: Seat not found.\n";
        return;
    }

    if (!seat->isAvailableSeat())
    {
        std::cout << "Error: Seat " << seatId << " is already occupied.\n";
        return;
    }

    // --- Payment ---
    std::string method = promptLine("Enter Payment Method (CreditCard/Cash/PayPal): ");
    std::shared_ptr<IPayment> payment = nullptr;

    if (method == "Cash" || method == "cash")
    {
        payment = std::make_shared<Cash>(seat->getSeatPrice());
    }
    else if (method == "CreditCard" || method == "creditcard")
    {
        std::string card = promptLine("Enter card number: ");
        payment = std::make_shared<CreditCard>(seat->getSeatPrice(), card);
    }
    else if (method == "PayPal" || method == "paypal")
    {
        std::string email = promptLine("Enter PayPal email: ");
        payment = std::make_shared<Paypal>(seat->getSeatPrice(), email);
    }
    else
    {
        std::cout << "Unknown payment method.\n";
        return;
    }


    // --- Delegate Booking ---
    auto reservation = agent->reserveForPassenger(passenger, flight, seat, payment);
    if (!reservation)
    {
        std::cout << "Booking failed. Please try again.\n";
        return;
    }

    std::cout << "\nBooking completed successfully!\n";

    // --- Persist updated flight (seat availability changed) ---
    flightRepo.updateById(flight->getFlightNumber(), flight->toJson());

} 

void Menu::agentModifyReservation(std::shared_ptr<BookingAgent> agent)
{
    if (!agent)
    {
        std::cout << "[Error] Invalid booking agent.\n";
        return;
    }

    std::string reservationId = promptLine("Enter Reservation ID to modify: ");

    // --- Load reservation ---
    ReservationRepository resRepo("data/reservations.json");
    auto rjson = resRepo.findById(reservationId);
    if (!rjson)
    {
        std::cout << "Error: Reservation not found.\n";
        return;
    }

    std::shared_ptr<Reservation> reservation;
    try
    {
        reservation = std::make_shared<Reservation>(Reservation::fromJson(*rjson));
    }
    catch (const std::exception& e)
    {
        std::cout << "Error constructing reservation: " << e.what() << std::endl;
        return;
    }

    if (!reservation)
    {
        std::cout << "Error: Could not load reservation data.\n";
        return;
    }

    auto flight = reservation->getFlight();
    if (!flight)
    {
        // reconstruct flight if needed
        FlightRepository frepo("data/flights.json");
        auto fjson = frepo.findById(reservation->getFlight()->getFlightNumber());
        if (!fjson)
        {
            std::cout << "Error: Related flight not found.\n";
            return;
        }
        flight = std::make_shared<Flight>(Flight::fromJson(*fjson));
    }

    if (!flight || !flight->getAircraft())
    {
        std::cout << "Error: Flight or aircraft data missing.\n";
        return;
    }

    // --- Get new seat ---
    std::string newSeatId = promptLine("Enter new seat number (e.g., 12A): ");
    auto newSeat = flight->getAircraft()->getSeatById(newSeatId);

    if (!newSeat)
    {
        std::cout << "Error: Seat not found.\n";
        return;
    }

    if (!newSeat->isAvailableSeat())
    {
        std::cout << "Error: Seat " << newSeatId << " is already occupied.\n";
        return;
    }

    std::cout << "Processing modification...\n";

    // --- Delegate to BookingAgent ---
    agent->modifyReservation(reservation, newSeat);
    std::cout << "Modification process completed.\n";

    FlightRepository frepo("data/flights.json");
    frepo.updateById(flight->getFlightNumber(), flight->toJson());

} 

void Menu::agentCancelReservation(std::shared_ptr<BookingAgent> agent)
{
    if (!agent)
    {
        std::cout << "[Error] Invalid booking agent reference.\n";
        return;
    }

    std::string reservationId = promptLine("Enter Reservation ID to cancel: ");

    // --- Load reservation from repository ---
    ReservationRepository resRepo("data/reservations.json");
    auto rjson = resRepo.findById(reservationId);
    if (!rjson)
    {
        std::cout << "Error: Reservation not found.\n";
        return;
    }

    std::shared_ptr<Reservation> reservation;
    try
    {
        reservation = std::make_shared<Reservation>(Reservation::fromJson(*rjson));
    }
    catch (const std::exception& e)
    {
        std::cout << "Error constructing reservation: " << e.what() << std::endl;
        return;
    }

    if (!reservation)
    {
        std::cout << "Error: Could not reconstruct reservation object.\n";
        return;
    }

    // --- Confirm cancellation ---
    std::string confirm = promptLine("Are you sure you want to cancel this reservation? (yes/no): ");
    if (confirm != "yes" && confirm != "y" && confirm != "Y")
    {
        std::cout << "Cancellation aborted.\n";
        return;
    }

    // --- Delegate to BookingAgent method ---
    std::cout << "Processing cancellation...\n";
    agent->cancelReservation(reservation);

    // --- Persist updated flight after freeing the seat ---
    auto flight = reservation->getFlight();
    if (flight)
    {
        FlightRepository frepo("data/flights.json");
        frepo.updateById(flight->getFlightNumber(), flight->toJson());
    }

}
*/


// ----------------- PASSENGER MENU -----------------
void Menu::passengerMenu(std::shared_ptr<Passenger> passenger)
{
    int choice = 0;
    do
    {
        std::cout << "\n--- Passenger Menu ---\n"
                  << "1. Search Flights\n"
                  << "2. Book a Flight\n"
                  << "3. View My Reservations\n"
                  << "4. Modify Reservation\n"
                  << "5. Cancel Reservation\n"
                  << "6. Check-In\n"
                  << "7. Logout\n"
                  << "Enter choice: ";

        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice)
        {
        case 1: passengerSearchFlights(passenger); break;
        case 2: passengerBookFlight(passenger); break;
        case 3: passengerViewReservations(passenger); break;
        case 4: passengerModifyReservation(passenger); break;
        case 5: passengerCancelReservation(passenger); break;
        case 6: passengerCheckIn(passenger); break;
        case 7: passenger->logout(); break;
        default: std::cout << "Invalid choice.\n"; break;
        }

    } while (choice != 7);
}



void Menu::passengerSearchFlights(std::shared_ptr<Passenger> passenger)
{
    std::string origin = promptLine("Enter Origin: ");
    std::string destination = promptLine("Enter Destination: ");
    std::string date = promptLine("Enter Departure Date (YYYY-MM-DD): ");

    FlightRepository frepo("data/flights.json");
    auto flights = frepo.findFlights(origin, destination, date);

    // delegate actual display to Passenger
    bool available = passenger->searchFlights(flights);
    if(available == false)
    {
        return;
    }

    std::string choice = promptLine("Do you want to book a flight [y/n]: ");
    while(true)
    {
        if(choice == "y" || choice == "Y")
        {
            passengerBookFlight(passenger);
            break;
        }
        else if(choice == "n" || choice == "N")
        {
            break;
        }
        else
        {
            choice = promptLine("Invalid input, please enter [y/n]: ");
        }
    }
}


void Menu::passengerBookFlight(std::shared_ptr<Passenger> passenger)
{
    std::string flightNumber = promptLine("Enter Flight Number: ");

    FlightRepository frepo("data/flights.json");
    auto fjson = frepo.findById(flightNumber);
    if (!fjson) {
        std::cout << "Flight not found.\n";
        return;
    }

    // Load existing flight state (with full aircraft + seat info)
    auto flight = Flight::fromJson(*fjson); 
    auto aircraft = flight.getAircraft();
    if (!aircraft) {
        std::cout << "No aircraft assigned.\n";
        return;
    }

    // Ensure this flight can still be booked
    if (flight.getStatus() == FlightStatus::Cancelled) {
        std::cout << "This flight is cancelled. Booking not allowed.\n";
        return;
    }
    if (flight.getStatus() == FlightStatus::Completed) {
        std::cout << "This flight has already been completed (Arrived).\n";
        return;
    }

    // -----------------------------
    // 1️⃣ Select seat class and seat
    // -----------------------------
    std::shared_ptr<Seat> chosenSeat = nullptr;

    while (true)
    {
        std::string seatClass = promptLine("Enter Seat Class [First/Business/Economy]: ");
        SeatClass selectedClass;

        if (seatClass == "First") selectedClass = SeatClass::First;
        else if (seatClass == "Business") selectedClass = SeatClass::Business;
        else if (seatClass == "Economy") selectedClass = SeatClass::Economy;
        else {
            std::cout << "Invalid input.\n";
            continue;
        }

        // Show available seats for this class
        std::cout << std::endl;
        flight.displaySeats(selectedClass);
        std::cout << std::endl;

        // Let the passenger choose
        std::string seatId = promptLine("Enter Seat Number (e.g., 12A) (0 to cancel): ");
        if(seatId == "0")
        {
            return;
        }
        auto seat = aircraft->getSeatById(seatId);

        if (!seat) {
            std::cout << "Invalid seat number.\n";
            continue;
        }

        if (seat->getSeatClass() != selectedClass) {
            std::cout << "Seat " << seatId << " is not in the selected class.\n";
            continue;
        }

        if (!seat->isAvailableSeat()) {
            std::cout << "Seat already occupied.\n";
            continue;
        }

        chosenSeat = seat;
        break; // Valid seat selected
    }

    // -----------------------------
    // 2️⃣ Choose payment method
    // -----------------------------
    std::string method = promptLine("Enter Payment Method (CreditCard/Cash/PayPal): ");
    std::shared_ptr<IPayment> payment = nullptr;

    if (method == "Cash" || method == "cash") {
        payment = std::make_shared<Cash>(chosenSeat->getSeatPrice());
    } 
    else if (method == "CreditCard" || method == "creditcard") {
        std::string card = promptLine("Enter card number: ");
        payment = std::make_shared<CreditCard>(chosenSeat->getSeatPrice(), card);
    } 
    else if (method == "PayPal" || method == "paypal") {
        std::string email = promptLine("Enter PayPal email: ");
        payment = std::make_shared<Paypal>(chosenSeat->getSeatPrice(), email);
    } 
    else {
        std::cout << "Unknown payment method.\n";
        return;
    }

    // -----------------------------
    // 3️⃣ Perform reservation
    // -----------------------------
    ReservationService reservationService;
    auto reservation = reservationService.reserve(passenger, std::make_shared<Flight>(flight), chosenSeat, payment, std::nullopt);

    if (!reservation) {
        std::cout << "Reservation failed.\n";
        return;
    }

    // -----------------------------
    // 4️⃣ Persist seat update to flights.json
    // -----------------------------
    chosenSeat->setAvailability(false);
    frepo.updateById(flight.getFlightNumber(), flight.toJson());

    std::cout << "\nSeat successfully booked and flight data updated!\n";
}


/*
void Menu::passengerBookFlight(std::shared_ptr<Passenger> passenger)
{
    std::string flightNumber = promptLine("Enter Flight Number: ");

    FlightRepository frepo("data/flights.json");
    auto fjson = frepo.findById(flightNumber);
    if (!fjson) { std::cout << "Flight not found.\n"; return; }
// the following line is the cause of every time the flight is reset -> all the seats are available
    auto flight = std::make_shared<Flight>(Flight::fromJson(*fjson));
    auto aircraft = flight->getAircraft();
    if (!aircraft) { std::cout << "No aircraft assigned.\n"; return; }

    while(true)
    {
        std::string seatClass = promptLine("Enter Seat Class [First/Business/Economy]: ");
        
        if(seatClass == "First")
        {
            flight->displaySeats(SeatClass::First);
            break;
        }
        else if(seatClass == "Business")
        {
            flight->displaySeats(SeatClass::Business);
            break;
        }
        else if(seatClass == "Economy")
        {
            flight->displaySeats(SeatClass::Economy);
            break;
        }
        else
        {
            std::cout << "Invalid Input." << std::endl;
        }
    }
// put the fllowing line inside while loop that check this seat id is within this seat class and it it is available 
    std::string seatId = promptLine("Enter Seat Number (e.g., 12A): ");
    std::string method = promptLine("Enter Payment Method (CreditCard/Cash/PayPal): ");


    auto seat = aircraft->getSeatById(seatId);
    if (!seat) { std::cout << "Invalid seat number.\n"; return; }
    if (!seat->isAvailableSeat()) { std::cout << "Seat already occupied.\n"; return; }

    std::shared_ptr<IPayment> payment = nullptr;
    if (method == "Cash" || method == "cash")
    {
        payment = std::make_shared<Cash>(seat->getSeatPrice());
    }
    else if (method == "CreditCard" || method == "creditcard")
    {
        std::string card = promptLine("Enter card number: ");
        payment = std::make_shared<CreditCard>(seat->getSeatPrice(), card);
    }
    else if (method == "PayPal" || method == "paypal")
    {
        std::string email = promptLine("Enter PayPal email: ");
        payment = std::make_shared<Paypal>(seat->getSeatPrice(), email);
    }
    else
    {
        std::cout << "Unknown payment method.\n";
        return;
    }


    // Delegate everything else (booking logic + couts) to Passenger::reserve()
    passenger->reserve(flight, seat, payment);

    seat->setAvailability(false); // mark as booked
    // optionally: seat->assignPassenger(passenger->getId());

    FlightRepository frepo("data/flights.json");
    frepo.updateById(flight->getFlightNumber(), flight->toJson());
}
*/

void Menu::passengerViewReservations(std::shared_ptr<Passenger> passenger)
{
    ReservationRepository rrepo("data/reservations.json");
    auto allJson = rrepo.getAll();

    std::vector<std::shared_ptr<Reservation>> reservations;
    reservations.reserve(allJson.size());
    for (const auto& r : allJson)
        reservations.push_back(std::make_shared<Reservation>(Reservation::fromJson(r)));

    passenger->viewMyReservations(reservations);
}


void Menu::passengerModifyReservation(std::shared_ptr<Passenger> passenger)
{
    std::string reservationId = promptLine("Enter Reservation ID: ");

    ReservationRepository rrepo("data/reservations.json");
    auto rjson = rrepo.findById(reservationId);

    if (!rjson)
    {
        std::cout << "Reservation not found.\n";
        return;
    }

    std::string flightNumber = rjson->at("flightNumber");
    std::string currentSeatId = rjson->at("seatId");

    FlightRepository frepo("data/flights.json");
    auto fjson = frepo.findById(flightNumber);
    if (!fjson)
    {
        std::cout << "Flight not found.\n";
        return;
    }

    // --- Load the full objects ---
    auto flight = std::make_shared<Flight>(Flight::fromJson(*fjson));
    auto aircraft = flight->getAircraft();
    if (!aircraft)
    {
        std::cout << "No aircraft assigned.\n";
        return;
    }

    // --- Rebuild reservation object from JSON ---
    auto reservation = std::make_shared<Reservation>(Reservation::fromJson(*rjson));

    auto oldSeat = aircraft->getSeatById(currentSeatId);
    if (!oldSeat)
    {
        std::cout << "Current seat not found in flight.\n";
        return;
    }

    SeatClass seatClass = oldSeat->getSeatClass();

    std::cout << "\n--- Modify Reservation ---\n";
    std::cout << "Current Seat: " << currentSeatId << " (" 
              << Seat::seatClassToString(seatClass) << ")\n\n";
    std::cout << "Available seats in " << Seat::seatClassToString(seatClass) << " class:\n\n";
    flight->displaySeats(seatClass);
    std::cout << std::endl;

    ReservationService reservationService;

    // --- Loop until valid new seat selected ---
    while (true)
    {
        std::string newSeatId = promptLine("Enter New Seat Number (e.g., 12A) (0 to cancel): ");
        if (newSeatId == "0")
            return;

        auto newSeat = aircraft->getSeatById(newSeatId);
        if (!newSeat)
        {
            std::cout << "Invalid seat ID.\n";
            continue;
        }

        // Use Seat::parseSeatId() for safety + consistency
        auto [row, col] = Seat::parseSeatId(newSeatId);
        newSeat->setRow(row);
        newSeat->setColumn(col);

        // --- Call your existing ReservationService method ---
        if (reservationService.modifyReservation(passenger, reservation, newSeat))
            break;  // success
        else
            std::cout << "\nTry again.\n\n";
    }
}



void Menu::passengerCancelReservation(std::shared_ptr<Passenger> passenger)
{
    std::string resId = promptLine("Enter Reservation ID: ");

    ReservationRepository rrepo("data/reservations.json");
    auto rjson = rrepo.findById(resId);
    if (!rjson) { std::cout << "Reservation not found.\n"; return; }

    auto reservation = std::make_shared<Reservation>(Reservation::fromJson(*rjson));

    // Delegate full cancellation logic + refund to Passenger::cancelReservation()
    passenger->cancelReservation(reservation);
}


void Menu::passengerCheckIn(std::shared_ptr<Passenger> passenger)
{
    std::string resId = promptLine("Enter Reservation ID: ");

    ReservationRepository rrepo("data/reservations.json");
    auto rjson = rrepo.findById(resId);
    if (!rjson) { std::cout << "Reservation not found.\n"; return; }

    auto reservation = std::make_shared<Reservation>(Reservation::fromJson(*rjson));

    // Delegate to Passenger::performCheckIn(), which calls ReservationService::checkIn()
    passenger->performCheckIn(reservation);
}


// ----------------- Utilities -----------------

int Menu::promptInt(const std::string& msg)
{
    int value;
    while (true)
    {
        std::cout << msg;

        // If previous extraction failed, clear it
        if (!(std::cin >> value))
        {
            std::cin.clear(); // reset failbit
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        // Clear leftover newline before returning
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return value;
    }
}

std::string Menu::promptLine(const std::string& msg)
{
    std::cout << msg;

    // If there's leftover newline before getline
    if (std::cin.peek() == '\n')
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string input;
    std::getline(std::cin, input);

    // Prevent trailing carriage returns (Windows)
    if (!input.empty() && input.back() == '\r')
        input.pop_back();

    return input;
}


json Menu::buildFlightJsonInteractive()
{
    json j;

    std::string flightNumber = promptLine("Enter Flight Number: ");
    std::string origin = promptLine("Enter Origin: ");
    std::string dest = promptLine("Enter Destination: ");
    std::string departure = promptLine("Enter Departure Date and Time (YYYY-MM-DD HH:MM): ");
    std::string arrival = promptLine("Enter Arrival Date and Time (YYYY-MM-DD HH:MM): ");
    std::string aircraftModel = promptLine("Enter Aircraft Type: ");
    int totalSeats = promptInt("Enter Total Seats: ");
    std::string gate = promptLine("Enter Gate: ");
    std::string status = promptLine("Enter Status (Scheduled/Delayed/Cancelled): ");

    j["flightNumber"] = flightNumber;
    j["origin"] = origin;
    j["destination"] = dest;
    j["departure"] = departure;
    j["arrival"] = arrival;

    // Create an Aircraft instance so its seats are initialized,
    // then serialize the full aircraft (including seats) into the flight JSON.
    // This ensures flights.json contains the aircraft: { modelName, totalSeats, seats: [...] } structure.
    try
    {
        auto aircraftPtr = Aircraft::creat(aircraftModel, totalSeats);
        if (aircraftPtr)
        {
            j["aircraft"] = aircraftPtr->toJson();
        }
        else
        {
            // fallback minimal aircraft info if creation fails for any reason
            j["aircraft"] = {
                {"modelName", aircraftModel},
                {"totalSeats", totalSeats},
                {"seats", json::array()}
            };
        }
    }
    catch (const std::exception& ex)
    {
        // If any exception occurs, keep a safe fallback but don't crash the menu
        j["aircraft"] = {
            {"modelName", aircraftModel},
            {"totalSeats", totalSeats},
            {"seats", json::array()}
        };
    }

    j["gate"] = gate;
    j["status"] = status;

    return j;
}




/*
json Menu::buildFlightJsonInteractive()
{
    json j;
    std::string flightNumber = promptLine("Enter Flight Number: ");
    std::string origin = promptLine("Enter Origin: ");
    std::string dest = promptLine("Enter Destination: ");
    std::string departure = promptLine("Enter Departure Date and Time (YYYY-MM-DD HH:MM): ");
    std::string arrival = promptLine("Enter Arrival Date and Time (YYYY-MM-DD HH:MM): ");
    std::string aircraftModel = promptLine("Enter Aircraft Type: ");
    int totalSeats = promptInt("Enter Total Seats: ");
    std::string gate = promptLine("Enter gate: ");
    std::string status = promptLine("Enter Status (Scheduled/Delayed/Cancelled): ");

    j["flightNumber"] = flightNumber;
    j["origin"] = origin;
    j["destination"] = dest;
    j["departure"] = departure;
    j["arrival"] = arrival;
    j["aircraftModel"] = aircraftModel;
    j["totalSeats"] = totalSeats;
    j["gate"] = gate;
    j["status"] = status;
    return j;
} 
*/

json Menu::buildAircraftJsonInteractive()
{
    json j;
    std::string model = promptLine("Enter Aircraft Model Name: ");
    int seats = promptInt("Enter Total seats: ");
    j["modelName"] = model;
    j["totalSeats"] = seats;
    // seats themselves are initialized by Aircraft::initializeSeats when loaded
    return j;
}