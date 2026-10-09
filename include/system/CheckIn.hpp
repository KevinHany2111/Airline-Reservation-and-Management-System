#ifndef CHECKIN_HPP
#define CHECKIN_HPP

#include <memory>
#include "IUser.hpp"
#include "Reservation.hpp"

class ReservationService;            // Forward declaration

class CheckIn
{
    friend class ReservationService;

private:
    static bool performCheckIn(const std::shared_ptr<Reservation>& reservation, const std::shared_ptr<IUser>& actor);
};


#endif