#ifndef PILOT_HPP
#define PILOT_HPP

#include <string>
#include "ICrewMember.hpp"

class Pilot : public ICrewMember
{
public:
    Pilot(const std::string& n);
    void displayInfo() const override;
};

#endif // PILOT_HPP