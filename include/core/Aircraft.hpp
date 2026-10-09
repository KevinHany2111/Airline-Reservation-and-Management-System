#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include "Seat.hpp"
#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
#include <string>

using json = nlohmann::json;

class Aircraft : public std::enable_shared_from_this<Aircraft>
{
private:
    std::string modelName;
    int totalSeats;
    std::vector<std::shared_ptr<Seat>> seats;


    // Private Constructor
    Aircraft(const std::string& name, int numSeats);

    // Private helpers to assign seates for a class
    int assignSeats(int numSeats, SeatClass cls, int startRow);
    int getSeatsPerRowForClass(SeatClass cls) const;
    int roundSeatsToFullRows(int seats, int seatsPerRow) const;

public:
    static std::shared_ptr<Aircraft> creat(const std::string& name, int numSeats);

    // Seat Initialization 
    void initializeSeats(); // Automatically distribute seats into classes with their number os seats per class

    // Getters
    const std::vector<std::shared_ptr<Seat>>& getSeats() const;
    std::string getModelName() const;
    std::shared_ptr<Seat> getSeatById(const std::string& id) const;

    // Helper function for flight (sold out or not)
    bool hasAvailableSeat() const; 

    // Utilities
    void displaySeatMap(SeatClass filterClass) const;

    // Helper methods for serialization and serialization (data base dealing)
    json toJson() const;
    static std::shared_ptr<Aircraft> fromJson(const json& j);

};


#endif // AIRCRAFT_HPP