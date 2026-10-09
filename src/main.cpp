#include <iostream>
#include <memory>
#include "system/AuthenticationManager.hpp"
#include "roles/IUser.hpp"
#include "system/Menu.hpp"

int main()
{
    // Paths used across your repo snippets
    const std::string adminPath = "data/administrators.json";
    const std::string agentPath = "data/bookingAgents.json";
    const std::string passengerPath = "data/passengers.json";
    const std::string flightsPath = "data/flights.json";
    const std::string aircraftPath = "data/aircrafts.json";
    const std::string reservationsPath = "data/reservations.json";

    AuthenticationManager auth(adminPath, agentPath, passengerPath);

    std::shared_ptr<IUser> currentUser = nullptr;

    while (true)
    {
        int choice = Menu::showMainMenu();

        if (choice == 1) // Login
        {
            currentUser = Menu::handleLogin(auth);
            if (!currentUser)
            {
                // login failed, loop back to main menu
                continue;
            }
        }
        else if (choice == 2) // Sign Up (Passenger or Booking Agent)
        {
            currentUser = Menu::handleSignUp();
            if (!currentUser)
                continue;
        }
        else if (choice == 3) // Exit
        {
            std::cout << "Exiting... Goodbye!\n";
            break;
        }
        else
        {
            std::cout << "Invalid choice. Try again.\n";
            continue;
        }

        // Role dispatch
        Roles r = currentUser->getRole();
        if (r == Roles::Administrator)
        {
            // Menu functions expect shared_ptr<Administrator> -> dynamic cast
            auto adminPtr = std::dynamic_pointer_cast<Administrator>(currentUser);
            if (!adminPtr)
            {
                std::cerr << "[Error] Logged-in user claimed Administrator role but object is not Administrator.\n";
                auth.logout();
                currentUser.reset();
                continue;
            }
            Menu::administratorMenu(adminPtr);
        }
        else if (r == Roles::BookingAgent)
        {
            auto agentPtr = std::dynamic_pointer_cast<BookingAgent>(currentUser);
            if (!agentPtr)
            {
                std::cerr << "[Error] Logged-in user claimed BookingAgent role but object is not BookingAgent.\n";
                auth.logout();
                currentUser.reset();
                continue;
            }
            Menu::bookingAgentMenu(agentPtr);
        }
        else if (r == Roles::Passenger)
        {
            auto passengerPtr = std::dynamic_pointer_cast<Passenger>(currentUser);
            if (!passengerPtr)
            {
                std::cerr << "[Error] Logged-in user claimed Passenger role but object is not Passenger.\n";
                auth.logout();
                currentUser.reset();
                continue;
            }
            Menu::passengerMenu(passengerPtr);
        }
        else
        {
            std::cerr << "Unknown role. Logging out.\n";
        }

        // After role menu returns -> log out
        auth.logout();
        currentUser.reset();
    }

    return 0;
}
