#include <iostream>
#include <iomanip>
#include "Passenger.hpp"
#include "ReservationService.hpp"
#include "Flight.hpp"
#include "Seat.hpp"
#include "Reservation.hpp"
#include "CheckIn.hpp"

// Constructor (new instance for signup)
Passenger::Passenger(const std::string& uname, const std::string& plainPwd) :
                    IUser(uname, "P" + IdGenerator::generateId() , plainPwd , Roles::Passenger, IsPasswordHashed::no ) {}

// Constructor (old instance for fromJson)
Passenger::Passenger(const std::string& uname, const std::string& id, const std::string& hashedPwd) :
                    IUser(uname, id, hashedPwd, Roles::Passenger, IsPasswordHashed::yes) {}

// Reserve: pass shared_from_this() so ReservationService receives the calling passenger
std::shared_ptr<Reservation> Passenger::reserve(
                    const std::shared_ptr<Flight>& flight,
                    const std::shared_ptr<Seat>& seat,
                    const std::shared_ptr<IPayment>& payment)
{
    return ReservationService::reserve(shared_from_this(), flight, seat, payment);
}


bool Passenger::searchFlights(
                const std::vector<std::shared_ptr<Flight>>& allFlights) const
{
    std::cout << std::endl << "Available Flights:" << std::endl;
    if(allFlights.empty())
    {
        std::cout << "No available flights found." << std::endl;
        return false;
    }
    int counter = 1;
    for(const auto& f : allFlights )
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
    return true;
}


void Passenger::viewMyReservations(const std::vector<std::shared_ptr<Reservation>>& allReservations) const
{
    std::cout << std::endl << "--- My Reservations ---" << std::endl
              << "Fetching reservstions for passenger " << getUserName() << "..." << "\n\n";
    
    int counter = 0;

    for(const auto& reservation : allReservations)
    {
        if(!reservation) {continue;}
        if(reservation->getPassengerId() == this->getId())
        {
            counter++;
            auto flight = reservation->getFlight();
            auto seat = reservation->getSeat();
            std::string status;
            if(reservation->isCancelled()) { status = "Canceled"; }
            else if(reservation->isCheckedIn()) { status = "Checked-In"; }
            else { status = "Confirmed"; }

            std::cout << counter << ". Reservation ID: " << reservation->getReservationId() << std::endl
                      <<"Flight: " << reservation->getFlight()->getFlightNumber() << " from " 
                      << reservation->getFlight()->getOrigin() << " to " << reservation->getFlight()->getDestination() << std::endl
                      << "Departure: " << reservation->getFlight()->getDepartureDateTimeString() << std::endl
                      << "Seat: " << reservation->getSeat()->getSeatID() << std::endl
                      << "Status: " << status << "\n\n";

        }
    }
    if(counter == 0)
    {
        std::cout << "You have no reservations." << std::endl;
    }
    
}

void Passenger::performCheckIn(const std::shared_ptr<Reservation>& reservation)
{
    if(!reservation)
    {
        std::cout << "Invalid reservstion." << std::endl;
        return;
    }

    if(ReservationService::checkIn(shared_from_this(), reservation))
    {
        std::cout << "Check-in completed successfully." << std::endl;
    }
    else
    {
        std::cout << "Check-in failed." << std::endl;
    }

}

void Passenger::modifyReservation(const std::shared_ptr<Reservation>& reservation,
                                  const std::shared_ptr<Seat>& newSeat)
{
    if (!reservation || !newSeat)
    {
        std::cout << "Invalid input for reservation modification.\n";
        return;
    }

    if (!ReservationService::modifyReservation(shared_from_this(), reservation, newSeat))
    {
        std::cout << "Modification failed.\n";
    }
}

void Passenger::cancelReservation(const std::shared_ptr<Reservation>& reservation)
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

    // Delegate logic to ReservationService (handles validation and freeing seat and processing refund)
    if (ReservationService::cancelReservation(shared_from_this(), reservation))
    {
        reservation->getPayment()->proccessRefund();
        std::cout << "Reservation [" << reservation->getReservationId()
                  << "] cancelled successfully by passenger." << std::endl;
        reservation->getPayment()->finishRefund();
    }
    else
    {
        std::cout << "Cancellation failed.\n";
    }
}

void Passenger::login()
{
    std::cout << "Logging in successfully, Passenger: " << getUserName() << std::endl;
}

void Passenger::logout()
{
    std::cout << "Logging out successfully, Passenger: " << getUserName() << std::endl;
}

//void Passenger::setId(const std::string& id) { userId = id; }