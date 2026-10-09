#include <iostream>
#include "IUser.hpp"

#include "Passenger.hpp"
#include "BookingAgent.hpp"
#include "Administrator.hpp"
/*
#include <functional>
#include <functional>
#include <stdexcept>
#include <algorithm>
*/


IUser::IUser(const std::string& uname , std::string id , const std::string& password , Roles rol, IsPasswordHashed status) : 
username(uname), role(rol), userId(id) 
{
    if(status == IsPasswordHashed::no)
    {
        passwordHash = hashPassword(password);
    }
    else
    {
        passwordHash = password;
    }
}

std::string IUser::getId() const
{
    return userId;
}

std::string IUser::getUserName() const 
{
    return username;
}
std::string IUser::getPasswordHash() const
{
    return passwordHash;
}
std::string IUser::getRoleStr() const
{
    switch (role)
    {
    case Roles::Administrator: return "Administrator";
    case Roles::BookingAgent:  return "BookingAgent";
    case Roles::Passenger:     return "Passenger";
    default:                   return "Unknown";
    }
}

Roles IUser::getRole() const
{
    return role;
}

std::string IUser::hashPassword(const std::string& plainPassword)
{
    std::hash<std::string> hasher;
    return std::to_string(hasher(plainPassword));
}

bool IUser::verifyPassword(const std::string& plainPassword) const
{
    return passwordHash == hashPassword(plainPassword);
}


/*
std::string roleToString(Roles role)
{
    switch (role)
    {
    case Roles::Administrator: return "Administrator";
    case Roles::BookingAgent:  return "BookingAgent";
    case Roles::Passenger:     return "Passenger";
    default:                   return "Unknown";
    }
}

Roles roleFromString(const std::string& str)
{
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "administrator") return Roles::Administrator;
    if (lower == "bookingagent")  return Roles::BookingAgent;
    if (lower == "passenger")     return Roles::Passenger;
    throw std::invalid_argument("Invalid role string: " + str);
}
*/
json IUser::toJson() const
{
    return {
        {"userId", userId},
        {"username", username},
        {"passwordHash", passwordHash},
        {"role", getRoleStr()}
    };
}

std::shared_ptr<IUser> IUser::fromJson(const json& j)
{
    // Default key for most user JSON files
    return IUser::fromJson(j, "userId");
}

std::shared_ptr<IUser> IUser::fromJson(const json& j, const std::string& key)
{
    std::string roleStr = j.at("role");
    std::string uname = j.at("username");
    std::string pwd = j.at("passwordHash");
    std::string id = j.at(key);

    if (roleStr == "Passenger")
        return std::make_shared<Passenger>(uname, id, pwd);
    else if (roleStr == "BookingAgent")
        return std::make_shared<BookingAgent>(uname, id , pwd);
    else
        return std::make_shared<Administrator>(uname, id , pwd);
}



