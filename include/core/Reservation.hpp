#ifndef RESERVATION_HPP
#define RESERVATION_HPP

#include <string>
#include <memory>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>
#include "IUser.hpp"
#include "Flight.hpp"
#include "Seat.hpp"
#include "IPayment.hpp"

using json = nlohmann::json;

class CheckIn;                                // Forward Declaration
class ReservationService;                     // Forward Declaration
class Menu;


class Reservation
{
    // Authorized Friend classes
    friend class CheckIn;
    friend class ReservationService;
    friend class Menu;

private:
    std::string reservationId;
    std::shared_ptr<IUser> passenger;
    std::optional<std::shared_ptr<IUser>> bookingAgent;
    std::shared_ptr<Flight> flight;
    std::shared_ptr<Seat> seat;
    bool checkedIn;
    bool cancelled;
    std::chrono::system_clock::time_point boardingTime;
    std::shared_ptr<IPayment> payment;

    // Private methods - can only be called by friends
    void setCheckedIn(bool val);
    void cancel();
    void setPayment(const std::shared_ptr<IPayment>& p);

    // Time Utilities
    void computeBoardingTime();
    std::string getBoardingTimeString() const;

public:
    //Reservation();
    Reservation(const std::shared_ptr<IUser>& p,
                const std::shared_ptr<Flight>& f, const std::shared_ptr<Seat>& s,
                const std::optional<std::shared_ptr<IUser>>& agent = std::nullopt);

    // Getters
    std::string getReservationId() const;
    std::string getPassengerId() const;
    std::string getPassengerName() const;
    std::optional<std::shared_ptr<IUser>> getBookinAgent() const;
    std::shared_ptr<Flight> getFlight() const;
    std::shared_ptr<Seat> getSeat() const;
    bool isCheckedIn() const;
    bool isCancelled() const;
    std::shared_ptr<IPayment> getPayment() const;

    // Helper methods for serialization and serialization (data base dealing)
    json toJson() const;
    static Reservation fromJson(const json& j);
    
};

#endif // RESERVATION_HPP