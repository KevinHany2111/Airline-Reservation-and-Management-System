#include <iostream>
#include <iomanip>
#include "BookingAgent.hpp"
#include "Passenger.hpp"
#include "Reservation.hpp"
#include "ReservationService.hpp"
#include "Flight.hpp"
#include "Seat.hpp"
#include "IPayment.hpp"

BookingAgent::BookingAgent(const std::string& uname, const std::string& plainPwd) : 
                           IUser(uname, "BA" + IdGenerator::generateId() , plainPwd, Roles::BookingAgent, IsPasswordHashed::no ) {}

BookingAgent::BookingAgent(const std::string& uname, const std::string& id, const std::string& hashedPwd) :
                            IUser(uname, id, hashedPwd, Roles::BookingAgent, IsPasswordHashed::yes ) {}

// 1. Search flights
// you need to bind this Flight Info to a static function in Menu class to call it in Passenger::..... 
void BookingAgent::searchFlights(const std::vector<std::shared_ptr<Flight>>& flights)
{
    std::cout << std::endl << "--- Available FLights ---" << std::endl;
    if(flights.empty())
    {
        std::cout << "No available flights found" << std::endl;
        return;
    }

    int counter = 1;

    for(const auto& f : flights )
    {
        if(!f) { continue; }
        
        std::cout << counter++ << ". Flight: " << f->getFlightNumber() << std::endl
                  << "   Departure: " << f->getDepartureDateTimeString() << std::endl
                  << "   Arrival: " << f->getArrivalDateTimeString() << std::endl
                  << "   Aircraft: " << f->getAircraft()->getModelName() << std::endl
                  << "   Available Seats: " << f->getAvailableSeatsCount() << std::endl
                  << "   Price by Class: " <<  std::endl
                  << "      - First Class:     $500" << std::endl
                  << "      - Business Class:  $350" << std::endl
                  << "      - Economy Class:   $200" << std::endl << std::endl
                  << "----------------------------------------------" << std::endl;
    }
}

// 2. Book (reserve) a flight for a passenger
std::shared_ptr<Reservation> BookingAgent::reserveForPassenger(
                    const std::shared_ptr<Passenger>& passenger,
                    const std::shared_ptr<Flight>& flight,
                    const std::shared_ptr<Seat>& seat,
                    const std::shared_ptr<IPayment>& payment)
{
    if( !passenger || !flight || !seat || !payment )
    {
        std::cout << "Error: Missing data for booking." << std::endl;
        return nullptr;
    }

    auto reservation = ReservationService::reserve(passenger, flight, seat, payment, shared_from_this());
    if(!reservation)
    {
        std::cout << "Booking failed. Please try again." <<std::endl;
        return nullptr;
    }

    std::cout << std::endl << "Booking Successful!" << std::endl;
    std::cout << "Reservation ID: " << reservation->getReservationId() << std::endl
              << "Passenger: " << reservation->getPassengerId() << std::endl
              << "Flight: " << reservation->getFlight()->getFlightNumber()
              << " from " << reservation->getFlight()->getOrigin() << " to " 
              << reservation->getFlight()->getDestination() << std::endl
              << "Seat: " << reservation->getSeat()->getSeatID() << std::endl
              << "Total Cost: $" << reservation->getPayment()->getAmount() << std::endl
              << "Payment Method: " << reservation->getPayment()->getMethodStr() << std::endl;

    return reservation;
}


void BookingAgent::modifyReservation(
                const std::shared_ptr<Reservation>& reservation,
                const std::shared_ptr<Seat>& newSeat)
{
    if (!reservation || !newSeat)
    {
        std::cout << "Invalid input for reservation modification.\n";
        return;
    }

    // Delegate all logic to ReservationService
    if (!ReservationService::modifyReservation(shared_from_this(), reservation, newSeat))
    {
        std::cout << "Modification failed.\n";
    }
}


void BookingAgent::cancelReservation(const std::shared_ptr<Reservation>& reservation)
{
    if (!reservation)
    {
        std::cout << "Invalid reservation reference.\n";
        return;
    }

    if (reservation->isCancelled())
    {
        std::cout << "Reservation already cancelled.\n";
        return;
    }

    // Delegate logic to ReservationService (handles all rules and seat freeing)
    if (ReservationService::cancelReservation(shared_from_this(), reservation))
    {
        reservation->getPayment()->proccessRefund();
        std::cout << "Reservation [" << reservation->getReservationId()
                  << "] cancelled successfully by booking agent." << std::endl;
        reservation->getPayment()->finishRefund();
    }
    else
    {
        std::cout << "Cancellation failed.\n";
    }
}

void BookingAgent::login()
{
    std::cout << "Logging in successfully, Booking Agent: " << getUserName() << std::endl;
}

void BookingAgent::logout()
{
    std::cout << "Logging out successfully, Booking Agent: " << getUserName() << std::endl;
}

//void BookingAgent::setId(const std::string& id) { userId = id; }



//in cancelReservation:
//you will need to return in IPayment to add refund() that will cout << Processing refund to PayPal account: mike.paypal@example.com
// that depends on the method (pure virtual function) 


