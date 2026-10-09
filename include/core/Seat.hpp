#ifndef SEAT_HPP
#define SEAT_HPP

#include <string>
#include <memory>
#include <nlohmann/json.hpp>


using json = nlohmann::json;

// Forward declarations
class Aircraft;
class Reservation;
class ReservationService;
class Menu;

// Enum for Seat Category
enum class SeatClass { First, Business, Economy };

class Seat
{
    // Authorized friend Classes
    friend class Menu;
    friend class Aircraft;
    friend class Reservation;
    friend class ReservationService;

private:
    int row;                              //ex: 12
    char column;                          //ex: 'A'      //12A
    SeatClass Class;                      //enum value intsead of string
    int seatPrice;
    bool isAvailable;                     //true if seat not reserved
    std::weak_ptr<Aircraft> aircraft;     //weak link to aircraft to avoid 

    // Private Setters for only Aircraft 
    void setRow(int r);
    void setColumn(char c);
    void setSeatClass(SeatClass cls);

    // Private methods for only Reservation
//    void setAvailability(bool availability);


public:
// Constructors
    Seat() = default ;
    Seat(int r, char c, SeatClass sc, int price ,const std::shared_ptr<Aircraft>& parent);

// Getters
    int getRow() const;
    char getColumn() const;
    SeatClass getSeatClass() const;
    std::string getSeatClassName() const;
    bool isAvailableSeat() const;
    std::string getSeatID() const;
    int getSeatPrice() const;

    void setAvailability(bool availability);

    static std::pair<int, char> parseSeatId(const std::string& seatId);

// Converters
    static std::string seatClassToString(SeatClass cls);
    static SeatClass seatClassFromString(const std::string& str);

// Utilities
    void Display() const;

    // Helper methods for serialization and serialization (data base dealing)
    json toJson() const;
    static std::shared_ptr<Seat> fromJson(const json& j);

};

#endif // SEAT_HPP