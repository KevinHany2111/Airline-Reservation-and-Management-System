#ifndef ICREWMEMBER_HPP
#define ICREWMEMBER_HPP

#include <string>
#include "IdGenerator.hpp"

enum class CrewRoles { Pilot, FlightAttendant };

class Flight;
class Menu;

class ICrewMember
{
    friend class Flight;
    friend class Menu;

protected:
    std::string name;
    std::string id;
    CrewRoles role;
public:
    ICrewMember(const std::string& n, const std::string& i, CrewRoles r);

    // Getters
    std::string getName() const;
    std::string getId() const;
    CrewRoles getRole() const;
    std::string getRoleStr() const;

    // Pure virtual function 
    virtual void displayInfo() const = 0;

    //polymorphic destructor
    virtual ~ICrewMember() = default;
};

#endif // ICREWMEMBER_HPP