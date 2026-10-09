#include "AuthenticationManager.hpp"
#include <iostream>

AuthenticationManager::AuthenticationManager(const std::string& adminPath,
                                             const std::string& agentPath,
                                             const std::string& passengerPath)
{
    adminRepo = std::make_shared<AdministratorRepository>(adminPath);
    agentRepo = std::make_shared<BookingAgentRepository>(agentPath);
    passengerRepo = std::make_shared<PassengerRepository>(passengerPath);
}
/*
std::shared_ptr<IUser> AuthenticationManager::login(Roles role,
                                                    const std::string& username,
                                                    const std::string& password)
{
    std::string hashed = IUser::hashPassword(password);
    std::optional<json> result;

    switch (role)
    {
    case Roles::Administrator:
        result = adminRepo->authenticate(username, hashed);
        break;
    case Roles::BookingAgent:
        result = agentRepo->authenticate(username, hashed);
        break;
    case Roles::Passenger:
        result = passengerRepo->authenticate(username, hashed);
        break;
    }

    if (!result)
    {
        std::cout << "Invalid username or password.\n";
        return nullptr;
    }

    json userData = *result;
    std::string id = userData.value("userId", "UNKNOWN");
    std::string uname = userData.value("username", "Unknown");

    switch (role)
    {
    case Roles::Administrator:
        currentUser = std::make_shared<Administrator>(uname, password);
        break;
    case Roles::BookingAgent:
        currentUser = std::make_shared<BookingAgent>(uname, password);
        break;
    case Roles::Passenger:
        currentUser = std::make_shared<Passenger>(uname, password);
        break;
    }

    std::cout << "Login successful! Welcome " << currentUser->getUserName() << "\n";
    return currentUser;
}
*/

std::shared_ptr<IUser> AuthenticationManager::login(Roles role,
                                                    const std::string& username,
                                                    const std::string& password)
{
    std::string hashed = IUser::hashPassword(password);
    std::optional<json> result;

    switch (role)
    {
    case Roles::Administrator:
        result = adminRepo->authenticate(username, hashed);
        break;
    case Roles::BookingAgent:
        result = agentRepo->authenticate(username, hashed);
        break;
    case Roles::Passenger:
        result = passengerRepo->authenticate(username, hashed);
        break;
    }

    if (!result)
    {
        std::cout << "Invalid username or password.\n";
        return nullptr;
    }

    json userData = *result;

    std::string id = userData.value("userId", "UNKNOWN");
    std::string uname = userData.value("username", "Unknown");
    std::string storedHash = userData.value("passwordHash", "");

    // now construct the user with the ID from JSON and mark the password as hashed
    switch (role)
    {
    case Roles::Administrator:
        currentUser = std::make_shared<Administrator>(
            uname, id, storedHash);
        break;

    case Roles::BookingAgent:
        currentUser = std::make_shared<BookingAgent>(
            uname, id, storedHash);
        break;

    case Roles::Passenger:
        currentUser = std::make_shared<Passenger>(
            uname, id, storedHash);
        break;
    }

    std::cout << "Login successful! Welcome " << currentUser->getUserName() << "\n";
    return currentUser;
}

void AuthenticationManager::logout()
{
    if (currentUser)
    {
        std::cout << "Logging out " << currentUser->getUserName() << "...\n";
        currentUser.reset();
    }
    else
    {
        std::cout << "No user currently logged in.\n";
    }
}

std::shared_ptr<IUser> AuthenticationManager::getCurrentUser() const
{
    return currentUser;
}

std::shared_ptr<IUser> AuthenticationManager::signIn(
    Roles role,
    const std::string& username,
    const std::string& password)
{
    //std::string hashed = IUser::hashPassword(password);

    bool success = false;

    switch (role)
    {
    case Roles::Passenger:
    {
        if (passengerRepo->findByUsername(username))
        {
            std::cout << "Username already exists for Passenger.\n";
            return nullptr;
        }

        // Construct Passenger (generates ID + default attributes)
        auto passenger = std::make_shared<Passenger>(username, password);

        // Convert to JSON using its own method
        json newUser = passenger->toJson();
        //newUser["password"] = hashed; // ensure we store hashed password, not plaintext

        // Save through repository
        success = passengerRepo->add(newUser);
        if (success)
            currentUser = passenger;
        break;
    }

    case Roles::BookingAgent:
    {
        if (agentRepo->findByUsername(username))
        {
            std::cout << "Username already exists for Booking Agent.\n";
            return nullptr;
        }

        auto agent = std::make_shared<BookingAgent>(username, password);

        json newUser = agent->toJson();
        //newUser["password"] = hashed;

        success = agentRepo->add(newUser);
        if (success)
            currentUser = agent;
        break;
    }

    case Roles::Administrator:
        std::cout << "Sign-in is not available for administrators.\n";
        return nullptr;
    }

    if (success)
    {
        std::cout << "Registration successful! Welcome, "
                  << currentUser->getUserName() << "\n";
        return currentUser;
    }
    else
    {
        std::cout << "Error: Failed to save user data.\n";
        return nullptr;
    }
}
