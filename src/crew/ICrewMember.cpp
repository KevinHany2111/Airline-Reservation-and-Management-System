#include <iostream>
#include "ICrewMember.hpp"

ICrewMember::ICrewMember(const std::string& n,const std::string& i, CrewRoles r) :
                        name(n), id(i), role(r) {}

std::string ICrewMember::getName() const { return name; }
std::string ICrewMember::getId() const { return id; }
CrewRoles ICrewMember::getRole() const { return role; }
std::string ICrewMember::getRoleStr() const 
{
    switch(role)
    {
        case CrewRoles::Pilot:           return "Pilot";
        case CrewRoles::FlightAttendant: return "FlightAttendant";
        default:                     return "Unknown";
    }
}