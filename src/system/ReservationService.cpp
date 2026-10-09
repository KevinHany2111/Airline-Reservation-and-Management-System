#include <iostream>
#include <sstream>
#include <iomanip>
#include "ReservationService.hpp"
#include "ReservationRepository.hpp"
#include "FlightRepository.hpp"

bool ReservationService::checkIn(const std::shared_ptr<IUser>& actor, const std::shared_ptr<Reservation>& reservation)
{
    if (!actor || !reservation)
    {
        std::cout << "Invalid Input!" << std::endl;
        return false;
    }

    if (actor->getRole() != Roles::Passenger)
    {
        std::cout << "Only a Passenger can perform check-in." << std::endl;
        return false;
    }

    // Only that specific Passenger can check in
    if (actor->getId() != reservation->getPassengerId())
    {
        std::cout << "Unauthorized: not your reservation." << std::endl;
        return false;
    }

    if (reservation->isCancelled())
    {
        std::cout << "Cannot check-in a cancelled reservation." << std::endl;
        return false;
    }

    if (reservation->isCheckedIn())
    {
        std::cout << "Reservation already checked in." << std::endl;
        return false;
    }

    // ✅ Perform check-in
    if (!CheckIn::performCheckIn(reservation, actor))
    {
        std::cout << "Check-in failed due to internal error." << std::endl;
        return false;
    }

    // ✅ Mark as checked-in in memory
    reservation->setCheckedIn(true);

    std::cout << "Check-in successful for reservation " 
              << reservation->getReservationId() << "!" << std::endl;

    // ✅ Persist updated reservation to JSON file
    {
        ReservationRepository repo("data/reservations.json");
        json updated = reservation->toJson();

        if (!repo.updateById(reservation->getReservationId(), updated))
        {
            std::cerr << "[ReservationService] Warning: failed to persist checked-in reservation "
                      << reservation->getReservationId() << std::endl;
        }
    }

    return true;
}


/*
bool ReservationService::checkIn(const std::shared_ptr<IUser>& actor, const std::shared_ptr<Reservation>& reservation)
{
    if( !actor || !reservation )
    {
        std::cout << "Invalid Input!" << std::endl;
        return false;
    }

    if( actor->getRole() != Roles::Passenger)
    {
        std::cout << "Only a Passenger who can make check in" << std::endl;
        return false;
    }

    // Only that specific Passenger who can check in 
    if(actor->getId() != reservation->getPassengerId())
    {
        std::cout << "Unauthorized: not your reservation." << std::endl;
        return false;
    }

    if(reservation->isCancelled())
    {
        std::cout << "Cannot check-in a cancelled reservation." << std::endl;
        return false;
    }

    if(reservation->isCheckedIn())
    {
        std::cout << "Reservation already checked in" << std::endl;
        return false;
    }

    return CheckIn::performCheckIn(reservation, actor);
}
*/bool ReservationService::cancelReservation(const std::shared_ptr<IUser>& actor,
                                           const std::shared_ptr<Reservation>& reservation)
{
    if (!actor || !reservation)
    {
        std::cout << "Invalid Input!\n";
        return false;
    }

    if (reservation->isCancelled())
    {
        std::cout << "Reservation already cancelled.\n";
        return false;
    }

    // ❌ Prevent cancelling after check-in
    if (reservation->isCheckedIn())
    {
        std::cout << "Cannot cancel a reservation that has already been checked in.\n";
        return false;
    }

    if(reservation->getFlight()->getStatus() == FlightStatus::Cancelled)
    {
        std::cout << "Can't cancel reservation from a cancelled flight" << std::endl;
        return false;
    }

    if(reservation->getFlight()->getStatus() == FlightStatus::Completed)
    {
        std::cout << "Can't cancel reservation from arrived flight (Completed)" << std::endl;
        return false;
    }

    Roles role = actor->getRole();
    std::string actorId = actor->getId();

    // Passenger cancels their own reservation
    if (role == Roles::Passenger && actorId == reservation->getPassengerId())
    {
        reservation->cancel();

        // Free up the seat
        if (auto seat = reservation->getSeat())
        {
            seat->setAvailability(true);
        }

        //std::cout << "Reservation: " << reservation->getReservationId()
        //         << " cancelled successfully by passenger.\n";

        // ✅ Persist updated reservation
    ReservationRepository repo("data/reservations.json");
    json updated = reservation->toJson();
    if (!repo.updateById(reservation->getReservationId(), updated))
    {
        std::cerr << "[ReservationService] Warning: Failed to persist cancelled reservation "
                  << reservation->getReservationId() << std::endl;
    }

    // ✅ Persist updated flight (seat became available)
    try
    {
        if (auto flight = reservation->getFlight())
        {
            FlightRepository flightRepo("data/flights.json");
            if (!flightRepo.updateById(flight->getFlightNumber(), flight->toJson()))
            {
                std::cerr << "[ReservationService] Warning: Failed to persist flight update after cancellation: "
                          << flight->getFlightNumber() << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ReservationService] Warning: Failed to persist cancelled reservation flight update: " << e.what() << std::endl;
    }

    return true;
    }

    // Booking agent cancels their customer's reservation
    if (role == Roles::BookingAgent && reservation->getBookinAgent().has_value())
    {
        auto bookingAgentPtr = reservation->getBookinAgent().value();
        if (bookingAgentPtr && bookingAgentPtr->getId() == actorId)
        {
            reservation->cancel();

            // Free up the seat
            if (auto seat = reservation->getSeat())
            {
                seat->setAvailability(true);
            }

            std::cout << "Reservation: " << reservation->getReservationId()
                      << " cancelled successfully by booking agent.\n";

            // ✅ Persist updated reservation
            ReservationRepository repo("data/reservations.json");
            json updated = reservation->toJson();
            if (!repo.updateById(reservation->getReservationId(), updated))
            {
                std::cerr << "[ReservationService] Warning: Failed to persist cancelled reservation "
                            << reservation->getReservationId() << std::endl;
            }

            // ✅ Persist updated flight (seat became available)
            try
            {
                if (auto flight = reservation->getFlight())
                {
                    FlightRepository flightRepo("data/flights.json");
                    if (!flightRepo.updateById(flight->getFlightNumber(), flight->toJson()))
                    {
                        std::cerr << "[ReservationService] Warning: Failed to persist flight update after cancellation: "
                                    << flight->getFlightNumber() << std::endl;
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "[ReservationService] Warning: Failed to persist cancelled reservation flight update: " << e.what() << std::endl;
            }

            return true;
        }

        std::cout << "Unauthorized: cannot cancel this reservation.\n";
        return false;
    }

    std::cout << "Unauthorized: role not permitted or booking agent mismatch.\n";
    return false;
}

/*
bool ReservationService::cancelReservation(const std::shared_ptr<IUser>& actor, const std::shared_ptr<Reservation>& reservation)
{
    if (!actor || !reservation)
    {
        std::cout << "Invalid Input!\n";
        return false;
    }

    if (reservation->isCancelled())
    {
        std::cout << "Reservation already cancelled.\n";
        return false;
    }

    Roles role = actor->getRole();
    std::string actorId = actor->getId();

    // Passenger cancels their own reservation
    if (role == Roles::Passenger && actorId == reservation->getPassengerId())
    {
        reservation->cancel();

        // Free up the seat
        if (auto seat = reservation->getSeat())
        {
            seat->setAvailability(true);
        }

        std::cout << "Reservation: " << reservation->getReservationId()
                  << " cancelled successfully by passenger.\n";
        return true;
    }

    // Booking agent cancels their customer's reservation
    if (role == Roles::BookingAgent && reservation->getBookinAgent().has_value())
    {
        auto bookingAgentPtr = reservation->getBookinAgent().value();
        if (bookingAgentPtr && bookingAgentPtr->getId() == actorId)
        {
            reservation->cancel();

            // Free up the seat
            if (auto seat = reservation->getSeat())
            {
                seat->setAvailability(true);
            }

            std::cout << "Reservation: " << reservation->getReservationId()
                      << " cancelled successfully by booking agent.\n";
            return true;
        }

        std::cout << "Unauthorized: cannot cancel this reservation.\n";
        return false;
    }

    std::cout << "Unauthorized: role not permitted or booking agent mismatch.\n";
    return false;
}

std::shared_ptr<Reservation> ReservationService::reserve(
                const std::shared_ptr<IUser>& passenger,
                const std::shared_ptr<Flight>& flight,
                const std::shared_ptr<Seat>& seat,
                const std::shared_ptr<IPayment>& payment,
                const std::optional<std::shared_ptr<IUser>>& bookingAgent)
{
    // Validate inputs
    if( !passenger || !flight || !seat || !payment )
    {
        std::cout << "Invalid Input for reservation" << std::endl;
        return nullptr;
    }

    if(passenger->getRole() != Roles::Passenger)
    {
        std::cout << "Error: Reservation must belong to a Passenger." << std::endl;
        return nullptr;
    }

    if(!bookingAgent.value() || bookingAgent.value()->getRole() != Roles::BookingAgent)
    {
        std::cout << "Error: Invalid booking agent" << std::endl;
        return nullptr;
    }
    
    auto reservation = std::make_shared<Reservation>(passenger, flight, seat, bookingAgent);
    reservation->setPayment(payment);


    std::cout << "Booking Successful!" << std::endl 
              << "Reservation ID: " << reservation->getReservationId() << std::endl
              << "Passenger: " << reservation->getPassengerName() << std::endl
              << "Flight: " << reservation->getFlight()->getFlightNumber() << " from " 
              << reservation->getFlight()->getOrigin() << " to " << reservation->getFlight()->getDestination() << std::endl
              << "Departure: " << reservation->getFlight()->getDepartureDateTimeString() << std::endl
              << "Seat: " << reservation->getSeat()->getSeatID() << std::endl
              << "Total Cost: $" << reservation->getSeat()->getSeatPrice() << std::endl
              << "Payment Method: " << reservation->getPayment()->getMethodStr() << std::endl;

    return reservation;
}
*/


std::shared_ptr<Reservation> ReservationService::reserve(
    const std::shared_ptr<IUser>& passenger,
    const std::shared_ptr<Flight>& flight,
    const std::shared_ptr<Seat>& seat,
    const std::shared_ptr<IPayment>& payment,
    const std::optional<std::shared_ptr<IUser>>& bookingAgent)
{
    // -----------------------------
    // 1️⃣ Validate inputs
    // -----------------------------
    if(!passenger)
    {
        std::cout << "[debug] : passenger null pointer" << std::endl;
    }
    if(!flight)
    {
        std::cout << "[debug] : flight null pointer"<< std::endl;
    }
    if(!seat)
    {
        std::cout << "[debug] : passenger null pointer"<< std::endl;
    }
    if(!payment)
    {
        std::cout << "[debug] : passenger null pointer"<< std::endl;
    }

    if (!passenger || !flight || !seat || !payment)
    {
        std::cout << "Invalid input for reservation.\n";
        return nullptr;
    }

    if (passenger->getRole() != Roles::Passenger)
    {
        std::cout << "Error: Reservation must belong to a Passenger.\n";
        return nullptr;
    }

    if (bookingAgent.has_value() && bookingAgent.value() && bookingAgent.value()->getRole() != Roles::BookingAgent)
    {
        std::cout << "Error: Invalid booking agent.\n";
        return nullptr;
    }

    // -----------------------------
    // 🧩 Verify flight status
    // -----------------------------
    if (flight->getStatus() != FlightStatus::Scheduled)
    {
        std::cout << "Error: Cannot reserve on a flight that is not scheduled.\n";
        return nullptr;
    }

    // -----------------------------
    // 2️⃣ Verify seat belongs to aircraft and is available
    // -----------------------------
    bool seatFound = false;
    for (const auto& s : flight->getAircraft()->getSeats())
    {
        if (s->getSeatID() == seat->getSeatID())
        {
            seatFound = true;
            if (!s->isAvailableSeat())
            {
                std::cout << "Error: Seat " << seat->getSeatID() << " is already reserved.\n";
                return nullptr;
            }
            break;
        }
    }

    if (!seatFound)
    {
        std::cout << "Error: The seat does not belong to this aircraft.\n";
        return nullptr;
    }

    // -----------------------------
    // 3️⃣ Reserve the seat and create the Reservation object
    // -----------------------------
    seat->setAvailability(false);

    auto reservation = std::make_shared<Reservation>(passenger, flight, seat, bookingAgent);
    reservation->setPayment(payment);

    // -----------------------------
    // ✅ 3.5 Persist updated Flight (seat availability)
    // -----------------------------
    try
    {
        FlightRepository flightRepo("data/flights.json");
        flightRepo.updateById(flight->getFlightNumber(), flight->toJson());
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ReservationService] Warning: Failed to update flight data: " << e.what() << '\n';
    }

    // -----------------------------
    // 4️⃣ Persist reservation
    // -----------------------------
    json j = reservation->toJson();
    ReservationRepository reservationRepo("data/reservations.json");
    reservationRepo.add(j);

    // -----------------------------
    // 5️⃣ Output confirmation
    // -----------------------------
    std::cout << "\nBooking Successful!" << std::endl
              << "Reservation ID: " << reservation->getReservationId() << std::endl
              << "Passenger: " << passenger->getUserName() << std::endl
              << "Flight: " << flight->getFlightNumber() << " (" 
              << flight->getOrigin() << " → " << flight->getDestination() << ")\n"
              << "Departure: " << flight->getDepartureDateTimeString() << std::endl
              << "Seat: " << seat->getSeatID() << std::endl
              << "Total Cost: $" << seat->getSeatPrice() << std::endl
              << "Payment Method: " << payment->getMethodStr() << std::endl;

    return reservation;
}




/*
std::shared_ptr<Reservation> ReservationService::reserve(
    const std::shared_ptr<IUser>& passenger,
    const std::shared_ptr<Flight>& flight,
    const std::shared_ptr<Seat>& seat,
    const std::shared_ptr<IPayment>& payment,
    const std::optional<std::shared_ptr<IUser>>& bookingAgent)
{
    // -----------------------------
    // 1️⃣ Validate inputs
    // -----------------------------
    if (!passenger || !flight || !seat || !payment)
    {
        std::cout << "Invalid input for reservation.\n";
        return nullptr;
    }

    if (passenger->getRole() != Roles::Passenger)
    {
        std::cout << "Error: Reservation must belong to a Passenger.\n";
        return nullptr;
    }

    if (bookingAgent.has_value() && bookingAgent.value() && bookingAgent.value()->getRole() != Roles::BookingAgent)
    {
        std::cout << "Error: Invalid booking agent.\n";
        return nullptr;
    }

// need to verify flight is sceduled or not 

    // -----------------------------
    // 2️⃣ Verify seat belongs to aircraft and is available
    // -----------------------------
    bool seatFound = false;
    for (const auto& s : flight->getAircraft()->getSeats())
    {
        if (s->getSeatID() == seat->getSeatID())
        {
            seatFound = true;
            if (!s->isAvailableSeat())
            {
                std::cout << "Error: Seat " << seat->getSeatID() << " is already reserved.\n";
                return nullptr;
            }
            break;
        }
    }

    if (!seatFound)
    {
        std::cout << "Error: The seat does not belong to this aircraft.\n";
        return nullptr;
    }

    // -----------------------------
    // 3️⃣ Reserve the seat and create the Reservation object
    // -----------------------------
    seat->setAvailability(false);

    auto reservation = std::make_shared<Reservation>(passenger, flight, seat, bookingAgent);
    reservation->setPayment(payment);

    // -----------------------------
    // 4️⃣ Persist through the repository layer
    // -----------------------------
    json j = reservation->toJson();
    ReservationRepository reservationRepo("data/reservations.json");
    reservationRepo.add(j);

    // -----------------------------
    // 5️⃣ Output confirmation
    // -----------------------------
    std::cout << "\nBooking Successful!" << std::endl
              << "Reservation ID: " << reservation->getReservationId() << std::endl
              << "Passenger: " << passenger->getUserName() << std::endl
              << "Flight: " << flight->getFlightNumber() << " (" 
              << flight->getOrigin() << " → " << flight->getDestination() << ")\n"
              << "Departure: " << flight->getDepartureDateTimeString() << std::endl
              << "Seat: " << seat->getSeatID() << std::endl
              << "Total Cost: $" << seat->getSeatPrice() << std::endl
              << "Payment Method: " << payment->getMethodStr() << std::endl;

    return reservation;
}
*/

bool ReservationService::modifyReservation(const std::shared_ptr<IUser>& actor,
                                           const std::shared_ptr<Reservation>& reservation,
                                           const std::shared_ptr<Seat>& newSeat)
{

    if(!actor)
    {
        std::cout << "[debug] actor null ptr" << std::endl;
    }
    if(!reservation)
    {
        std::cout << "[debug] reservation null ptr" << std::endl;
    }
    if(!newSeat)
    {
        std::cout << "[debug] newSeat null ptr" << std::endl;
    }
    
    if (!actor || !reservation || !newSeat)
    {
        std::cout << "Invalid input.\n";
        return false;
    }

    if (reservation->isCancelled())
    {
        std::cout << "Cannot modify a cancelled reservation.\n";
        return false;
    }

    Roles role = actor->getRole();

    // --- Authorization ---
    
    if (role == Roles::BookingAgent)
{
    // ✅ Safely verify ownership from JSON instead of using possibly invalid pointer
    ReservationRepository rrepo("data/reservations.json");
    auto storedJson = rrepo.findById(reservation->getReservationId());

    if (!storedJson)
    {
        std::cout << "Error: Reservation not found in repository.\n";
        return false;
    }

    std::string agentIdInFile = storedJson->value("bookingAgentId", "");

    if (agentIdInFile.empty())
    {
        std::cout << "Unauthorized: Reservation has no associated booking agent.\n";
        return false;
    }

    if (agentIdInFile != actor->getId())
    {
        std::cout << "Unauthorized: You did not create this reservation.\n";
        return false;
    }

    // ✅ Authorized — continue
}
    else if (role == Roles::Passenger)
    {
        if (reservation->getPassengerId() != actor->getId())
        {
            std::cout << "Unauthorized: You can only modify your own reservations.\n";
            return false;
        }
    }
    else
    {
        std::cout << "Unauthorized role: only passengers or booking agents can modify reservations.\n";
        return false;
    }

    // --- Validation of flight and aircraft ---
    auto flight = reservation->getFlight();
    if (!flight || !flight->getAircraft())
    {
        std::cout << "Error: Flight or aircraft missing.\n";
        return false;
    }

    if (flight->getStatus() == FlightStatus::Cancelled)
    {
        std::cout << "Error: can't modify reservation in a cancelled flight.\n";
        return false;
    }

    if (flight->getStatus() == FlightStatus::Completed)
    {
        std::cout << "Error: flight has already arrived (Completed).\n";
        return false;
    }

    // --- Validate that the new seat belongs to the same flight ---
    bool seatFound = false;
    for (const auto& s : flight->getAircraft()->getSeats())
    {
        if (s && s->getSeatID() == newSeat->getSeatID())
        {
            seatFound = true;

            if (!s->isAvailableSeat())
            {
                std::cout << "Error: Seat " << newSeat->getSeatID() << " is already reserved.\n";
                return false;
            }

            if (reservation->getSeat() &&
                reservation->getSeat()->getSeatClass() != newSeat->getSeatClass())
            {
                std::cout << "Error: Cannot change to a different seat class.\n";
                return false;
            }

            break;
        }
    }

    if (!seatFound)
    {
        std::cout << "Error: Seat does not belong to this flight.\n";
        return false;
    }

    // --- 🧠 Update real flight seat availability in flights.json ---
    {
        FlightRepository flightRepo("data/flights.json");
        auto fjson = flightRepo.findById(flight->getFlightNumber());
        if (!fjson)
        {
            std::cerr << "[modifyReservation] Flight not found in repository.\n";
            return false;
        }

        // Load the actual flight from file
        auto realFlight = std::make_shared<Flight>(Flight::fromJson(*fjson));
        auto aircraft = realFlight->getAircraft();
        if (!aircraft)
        {
            std::cerr << "[modifyReservation] Flight has no aircraft.\n";
            return false;
        }


        if (!reservation->getSeat())
        {
            std::cerr << "[modifyReservation] Reservation has no seat object.\n";
            return false;
        }

        // Locate both seats in the real flight
        auto oldSeat = aircraft->getSeatById(reservation->getSeat()->getSeatID());
        auto freshNewSeat = aircraft->getSeatById(newSeat->getSeatID());

        if (!oldSeat || !freshNewSeat)
        {
            std::cerr << "[modifyReservation] Seat lookup failed.\n";
            return false;
        }

        // Flip availability flags
        oldSeat->setAvailability(true);
        freshNewSeat->setAvailability(false);

        // Assign new seat to reservation
        reservation->seat = freshNewSeat;

        // Save updated flight to file
        if (!flightRepo.updateById(realFlight->getFlightNumber(), realFlight->toJson()))
        {
            std::cerr << "[modifyReservation] Failed to update flights.json\n";
        }
    }

    // --- 🧩 Persist updated reservation ---
    {
        ReservationRepository repo("data/reservations.json");
        json updated = reservation->toJson();

        // Make sure only seatId is stored, not full seat object
        updated.erase("seat");
        updated["seatId"] = reservation->getSeat()->getSeatID();

        if (!repo.updateById(reservation->getReservationId(), updated))
        {
            std::cerr << "[modifyReservation] Failed to update reservations.json\n";
        }
    }

    std::cout << "Reservation modified successfully. New seat: "
              << newSeat->getSeatID() << "\n";

    return true;
}
