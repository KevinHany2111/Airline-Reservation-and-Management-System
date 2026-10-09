#include <iostream>
#include <iomanip>
#include <sstream>
#include "Reservation.hpp"
#include "IdGenerator.hpp"
#include "Passenger.hpp"
#include "BookingAgent.hpp"
#include "Cash.hpp"
#include "CreditCard.hpp"
#include "PayPal.hpp"
#include "PassengerRepository.hpp"
#include "FlightRepository.hpp"
#include "BookingAgentRepository.hpp"

//Reservation::Reservation() : 
//                                passenger(nullptr),
//                                flight(nullptr),
//                                seat(nullptr),
//                                payment(nullptr),
//                                cancelled(false),
//                                checkedIn(false),
//                                boardingTime(std::chrono::system_clock::now()) {}

Reservation::Reservation(const std::shared_ptr<IUser>& p,
                         const std::shared_ptr<Flight>& f, const std::shared_ptr<Seat>& s,
                         const std::optional<std::shared_ptr<IUser>>& agent) :
                         passenger(p), flight(f), seat(s),
                         bookingAgent(agent) ,checkedIn(false), cancelled(false), payment(nullptr) 
{
    reservationId = "R" + IdGenerator::generateId();
}

// Getters
std::string Reservation::getReservationId() const { return reservationId; }
std::string Reservation::getPassengerId() const { return (passenger)? passenger->getId() : "-1"; }
std::string Reservation::getPassengerName() const { return (passenger) ? passenger->getUserName() : "Unknown" ;}
std::optional<std::shared_ptr<IUser>> Reservation::getBookinAgent() const { return bookingAgent; }
std::shared_ptr<Flight> Reservation::getFlight() const { return flight; }
std::shared_ptr<Seat> Reservation::getSeat() const { return seat; }
bool Reservation::isCheckedIn() const { return checkedIn; }
bool Reservation::isCancelled() const { return cancelled; }
std::shared_ptr<IPayment> Reservation::getPayment() const { return payment; }

// Private methods - can only be called by friends
void Reservation::setCheckedIn(bool val) { checkedIn = val; }
void Reservation::cancel() { cancelled = true; }
void Reservation::setPayment(const std::shared_ptr<IPayment>& p) { payment = p; }


// Time Utilities
void Reservation::computeBoardingTime()
{
    if(!flight) return;
    auto dep = flight->getDepartureTimePoint();
    boardingTime = dep - std::chrono::minutes(90);
}
std::string Reservation::getBoardingTimeString() const
{
    std::time_t bt = std::chrono::system_clock::to_time_t(boardingTime);
    std::tm* tmPtr = std::localtime(&bt);
    std::ostringstream oss;
    oss << std::put_time(tmPtr, "%Y-%m-%d %H:%M");
    return oss.str();
}

// Helper methods for serialization and serialization (data base dealing)
json Reservation::toJson() const
{
    json j;
    j["reservationId"] = reservationId;
    j["passengerId"] = getPassengerId();
    j["flightNumber"] = (flight) ? flight->getFlightNumber() : "";
    j["seatId"] = (seat) ? seat->getSeatID() : "";
    j["checkedIn"] = checkedIn;
    j["cancelled"] = cancelled;
    j["departureTime"] = flight->getDepartureDateTimeString();
    j["boardingTime"] = getBoardingTimeString();

    if (bookingAgent.has_value() && bookingAgent.value())
        { j["bookingAgentId"] = bookingAgent.value()->getId(); }

    if (payment) 
    {
        j["payment"]["method"] = payment->getMethodStr();
        j["payment"]["amount"] = payment->getAmount();

        if (payment->getMethodStr() == "CreditCard")
            { j["payment"]["cardNumber"] = static_cast<CreditCard*>(payment.get())->getCardNumber(); }
        else if (payment->getMethodStr() == "PayPal")
            { j["payment"]["email"] = static_cast<Paypal*>(payment.get())->getEmail(); }
    }

    return j;
}
/*
Reservation Reservation::fromJson(const json& j) {
    std::shared_ptr<IUser> passenger = nullptr;
    std::shared_ptr<IUser> agent = nullptr;
    std::shared_ptr<Flight> flight = nullptr;
    std::shared_ptr<Seat> seat = nullptr;

    // Minimal reconstruction (repositories will handle linking)
    auto reservation = Reservation(passenger, flight, seat, agent);

    if (j.contains("reservationId")) { reservation.reservationId = j["reservationId"]; }
    if (j.contains("checkedIn")) { reservation.checkedIn = j["checkedIn"]; }
    if (j.contains("cancelled")) { reservation.cancelled = j["cancelled"]; }

    if (j.contains("passengerId"))
        reservation.passenger = j.at("passengerId").get<std::string>();
    else if (j.contains("userId"))
        reservation.passengerId = j.at("userId").get<std::string>();
    else
        reservation.passengerId = "";

    // Payment handling
    if (j.contains("payment")) 
    {
        std::string method = j["payment"].value("method", "");
        double amount = j["payment"].value("amount", 0.0);

        if (method == "Cash")
            { reservation.payment = std::make_shared<Cash>(amount); }
        else if (method == "CreditCard")
            { reservation.payment = std::make_shared<CreditCard>(amount, j["payment"].value("cardNumber", "")); }
        else if (method == "PayPal")
            { reservation.payment = std::make_shared<Paypal>(amount, j["payment"].value("email", "")); }
    }

    return reservation;
}

*/

Reservation Reservation::fromJson(const json& j)
{
    std::shared_ptr<IUser> passenger = nullptr;
    std::shared_ptr<IUser> agent = nullptr;
    std::shared_ptr<Flight> flight = nullptr;
    std::shared_ptr<Seat> seat = nullptr;
    std::shared_ptr<IPayment> payment = nullptr;

    // Create empty reservation
    Reservation reservation(passenger, flight, seat, agent);

    try
    {
        // ----------------- Basic fields -----------------
        reservation.reservationId = j.value("reservationId", "");
        reservation.cancelled = j.value("cancelled", false);
        reservation.checkedIn = j.value("checkedIn", false);
        reservation.boardingTime = Flight::parseDateTime(
            j.value("boardingTime", "2025-11-02 07:30"));

        // ----------------- Load Passenger -----------------
        if (j.contains("passengerId"))
        {
            std::string pid = j["passengerId"].get<std::string>();

            PassengerRepository prepo("data/passengers.json");
            auto pjson = prepo.findById(pid);

            if (pjson)
            {
                // Pass the correct key name for passenger
                reservation.passenger = IUser::fromJson(*pjson, "userId");

                // Make sure ID matches the reservation
                if (reservation.passenger)
                    reservation.passenger->setId(pid);
            }
            else
            {
                std::cerr << "[Reservation::fromJson] Warning: Passenger with ID "
                          << pid << " not found.\n";
            }
        }

        // ----------------- Load Booking Agent (optional) -----------------
        if (j.contains("agentId"))
        {
            std::string aid = j["agentId"].get<std::string>();

            BookingAgentRepository arepo("data/bookingAgents.json");
            auto ajson = arepo.findById(aid);

            if (ajson)
            {
                // Pass the correct key name for agent
                reservation.bookingAgent = IUser::fromJson(*ajson, "userId");

                if (reservation.bookingAgent && reservation.bookingAgent.value())
                    reservation.bookingAgent.value()->setId(aid);
            }
            else
            {
                std::cerr << "[Reservation::fromJson] Warning: Agent with ID "
                          << aid << " not found.\n";
            }
        }

        // ----------------- Load Flight -----------------
        if (j.contains("flightNumber"))
        {
            std::string fno = j["flightNumber"].get<std::string>();
            FlightRepository frepo("data/flights.json");
            auto fjson = frepo.findById(fno);

            if (fjson)
            {
                reservation.flight = std::make_shared<Flight>(
                    Flight::fromJson(*fjson));

                // ----------------- Load Seat -----------------
                if (j.contains("seatId") && reservation.flight)
                {
                    std::string sid = j["seatId"].get<std::string>();
                    auto aircraft = reservation.flight->getAircraft();

                    if (aircraft)
                    {
                        auto seatObj = aircraft->getSeatById(sid);
                        if (seatObj)
                            reservation.seat = seatObj;
                        else
                            std::cerr << "[Reservation::fromJson] Seat " << sid
                                      << " not found in aircraft for flight " << fno << ".\n";
                    }
                    else
                    {
                        std::cerr << "[Reservation::fromJson] Aircraft missing for flight "
                                  << fno << ".\n";
                    }
                }
            }
            else
            {
                std::cerr << "[Reservation::fromJson] Flight " << fno << " not found.\n";
            }
        }

        // ----------------- Load Payment -----------------
        if (j.contains("payment") && j["payment"].is_object())
        {
            auto p = j["payment"];
            double amount = p.value("amount", 0.0);
            std::string method = p.value("method", "Cash");

            if (method == "CreditCard")
                payment = std::make_shared<CreditCard>(amount, "****");
            else if (method == "PayPal")
                payment = std::make_shared<Paypal>(amount, "unknown@paypal");
            else
                payment = std::make_shared<Cash>(amount);

            reservation.payment = payment;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Reservation::fromJson] Exception: " << e.what() << "\n";
    }

    return reservation;
}