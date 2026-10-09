#include <iostream>
#include "Pilot.hpp"

Pilot::Pilot(const std::string& n) : 
            ICrewMember(n, "PL" + IdGenerator::generateId() , CrewRoles::Pilot) {}
void Pilot::displayInfo() const
{
    std::cout << "Pilot: " << getName() << " ,ID: " << getId() << std::endl; 
}