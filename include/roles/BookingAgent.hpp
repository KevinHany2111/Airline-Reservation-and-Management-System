#ifndef BOOKINGAGENT_HPP
#define BOOKINGAGENT_HPP

#include "IUser.hpp"
#include <memory>
#include <string>
#include <vector>

class Passenger;
class Flight;
class Seat;
class Reservation;
class IPayment;

class BookingAgent : public IUser , public std::enable_shared_from_this<BookingAgent>
{
public:
    // Constructor (new instance for signup) , pwd isn't hashed
    BookingAgent(const std::string& uname, const std::string& plainPwd);
    // Constructor (old instance for fromJson), pwd is hashed
    BookingAgent(const std::string& uname, const std::string& id, const std::string& hahsedPwd);

    // 1. Search flights (display only; filtering done externally)
    void searchFlights(const std::vector<std::shared_ptr<Flight>>& flights);

    // 2. Book a flight on behalf of a passenger
    std::shared_ptr<Reservation> reserveForPassenger(
                const std::shared_ptr<Passenger>& passenger,
                const std::shared_ptr<Flight>& flight,
                const std::shared_ptr<Seat>& seat,
                const std::shared_ptr<IPayment>& payment);
    
    // 3. Modify existing reservation
    void modifyReservation(
                const std::shared_ptr<Reservation>& reservation,
                const std::shared_ptr<Seat>& newSeat);

    // 4. Cancel existing reservation
    void cancelReservation(const std::shared_ptr<Reservation>& reservation);

    // overriding the pure viryual functions in IUser 
    void login() override;
    void logout() override;
//    void setId(const std::string& id) override;



};


#endif