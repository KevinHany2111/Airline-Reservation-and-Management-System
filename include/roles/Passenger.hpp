#ifndef PASSENGER_HPP
#define PASSENGER_HPP

#include <memory>
#include <vector>
#include "IUser.hpp"

class Flight;                  // Forward Declarations
class Seat;
class Reservation;
class IPayment;

class Passenger : public IUser , public std::enable_shared_from_this<Passenger>
{
public:
    // Constructor (new instance for signup) , pwd isn't hashed
    Passenger(const std::string& uname, const std::string& plainPwd);
    // Constructor (old instance for fromJson), pwd is hashed
    Passenger(const std::string& uname, const std::string& id, const std::string& hahsedPwd);

    // Reserve for self
    std::shared_ptr<Reservation> reserve(
                const std::shared_ptr<Flight>& flight,
                const std::shared_ptr<Seat>& seat,
                const std::shared_ptr<IPayment>& payment);

    // Search for availabe flights
    bool searchFlights(
                const std::vector<std::shared_ptr<Flight>>& allFlights) const;
    
    // View reservations owned by this passenger
    void viewMyReservations(const std::vector<std::shared_ptr<Reservation>>& allReservations) const;

    // modify reservation (added new to the passenger menu to allow him modifying his reservations)
    void modifyReservation(const std::shared_ptr<Reservation>& reservation,
                           const std::shared_ptr<Seat>& newSeat);
    
    // cancel reservation (added new to the passenger menu to allow him cancelling his reservations)
    void cancelReservation(const std::shared_ptr<Reservation>& reservation);

    //View reservations owned by this passenger
    void performCheckIn(const std::shared_ptr<Reservation>& reservation);

    // overriding the pure viryual functions in IUser 
    void login() override;
    void logout() override;
//    void setId(const std::string& id) override;

};

#endif // PASSENGER_HPP