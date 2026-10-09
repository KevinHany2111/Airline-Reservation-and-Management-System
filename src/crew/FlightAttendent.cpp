#include <iostream>
#include "FlightAttendent.hpp"

FlightAttendant::FlightAttendant(const std::string& n) :
                                ICrewMember(n, "FA" + IdGenerator::generateId() , CrewRoles::FlightAttendant) {}

void FlightAttendant::displayInfo() const 
{
    std::cout << "Flight Attendant: " << getName() << " ,ID: " << getId() << std::endl;
}