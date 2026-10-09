#ifndef IUSER_HPP
#define IUSER_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "IdGenerator.hpp"

using json = nlohmann::json; 

enum class Roles { Administrator, BookingAgent, Passenger};
enum class IsPasswordHashed {yes, no};

class IUser
{
protected:
    std::string userId;
    std::string username;
    std::string passwordHash;
    Roles role;

public:
    IUser(const std::string& uname , std::string id , const std::string& password , Roles rol, IsPasswordHashed status);

    //Delete copy constructor and assignment operator
    IUser(const IUser& ) = delete;
    IUser& operator=(const IUser&) = delete;

    //Getters
    std::string getId() const;
    std::string getUserName() const ;
    std::string getPasswordHash() const;
    std::string getRoleStr() const;
    Roles getRole() const;

    virtual void setId(const std::string& id) { userId = id ;}

    // Password Utils
    bool verifyPassword(const std::string& plainPassword) const;
    static std::string hashPassword(const std::string& plainPassword);

    // Helper methods for serialization and serialization (data base dealing)
    virtual json toJson() const;
    static std::shared_ptr<IUser> fromJson(const json& j);
    static std::shared_ptr<IUser> fromJson(const json& j, const std::string& key);
/*
    // Helper conversion utilities
    static std::string roleToString(Roles role);
    static Roles roleFromString(const std::string& str);
*/
    // Pure virtual functions
    virtual void login() = 0 ;
    virtual void logout() = 0;

    //polymorphic destructor
    virtual ~IUser() = default;
};

#endif