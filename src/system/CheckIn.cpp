#include "CheckIn.hpp"
#include <iostream>

bool CheckIn::performCheckIn(const std::shared_ptr<Reservation>& reservation, const std::shared_ptr<IUser>& actor)
{
    reservation->computeBoardingTime();
    reservation->setCheckedIn(true);

    // Print Boarding Pass
    std::cout << std::endl << "Check-In Successfully!" << std::endl;
    std::cout << "Boarding Pass: " << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Reservation: " << reservation->getReservationId() << std::endl;
    std::cout << "Passenger: " << reservation->getPassengerName() << std::endl;
    std::cout << "Flight: " << reservation->getFlight()->getFlightNumber() << std::endl;
    std::cout << "Origin: " << reservation->getFlight()->getOrigin() << std::endl;
    std::cout << "Destination: " << reservation->getFlight()->getDestination() << std::endl;
    std::cout << "Departure: " << reservation->getFlight()->getDepartureDateTimeString() << std::endl;
    std::cout << "Seat: " << reservation->getSeat()->getSeatID() << std::endl;
    std::cout << "Gate:" << reservation->getFlight()->getGate() << std::endl;
    std::cout << "Borading Time: " << reservation->getBoardingTimeString() << std::endl;
    std::cout << "-----------------------------" << std::endl;

    return true;
}