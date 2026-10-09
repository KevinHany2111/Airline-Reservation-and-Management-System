#ifndef RESERVATIONSERVICE_HPP
#define RESERVATIONSERVICE_HPP

#include <memory>
#include <optional>
#include "CheckIn.hpp"
#include "Flight.hpp"
#include "IPayment.hpp"
#include "Passenger.hpp"
#include "BookingAgent.hpp"


class ReservationService
{
public:
    // Controlled entry points for sensitive operations

    static std::shared_ptr<Reservation> reserve(
                    const std::shared_ptr<IUser>& passenger,
                    const std::shared_ptr<Flight>& flight,
                    const std::shared_ptr<Seat>& seat,
                    const std::shared_ptr<IPayment>& payment,
                    const std::optional<std::shared_ptr<IUser>>& bookingAgent = std::nullopt);

    static bool checkIn(const std::shared_ptr<IUser>& actor, const std::shared_ptr<Reservation>& reservation);

    static bool cancelReservation(const std::shared_ptr<IUser>& actor, const std::shared_ptr<Reservation>& reservation);

    static bool modifyReservation(const std::shared_ptr<IUser>& actor, 
                                  const std::shared_ptr<Reservation>& reservation,
                                  const std::shared_ptr<Seat>& newSeat);
};

#endif // RESERVATIONSERVICE_HPP