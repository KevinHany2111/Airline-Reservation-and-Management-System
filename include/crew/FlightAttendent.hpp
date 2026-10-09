#ifndef FLIGHTATTENDANT_HPP
#define FLIGHTATTENDANT_HPP

#include <string>
#include "ICrewMember.hpp"

class FlightAttendant : public ICrewMember
{
public:
    FlightAttendant(const std::string& n);
    void displayInfo() const override;
};

#endif // FLIGHTATTENDANT_HPP