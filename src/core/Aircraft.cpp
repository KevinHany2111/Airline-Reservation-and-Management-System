#include <iostream>
#include "Aircraft.hpp"
#include <cmath>         //for ceil()
#include <numeric>       //for iota()
#include <iomanip>       //for setw()
#include <set>


// Private constructor
Aircraft::Aircraft(const std::string& name, int numSeats) :
modelName(name) , totalSeats(numSeats) {}

// Factory method - to construct Aircraft to make sure it is owned by shared_ptr
std::shared_ptr<Aircraft> Aircraft::creat(const std::string& name, int numSeats)
{
    std::shared_ptr<Aircraft> aircraft(new Aircraft(name, numSeats));
    aircraft->initializeSeats();
    return aircraft;
}



int Aircraft::getSeatsPerRowForClass(SeatClass cls) const
{
    switch(cls) 
    {
        case SeatClass::First:    return 4;
        case SeatClass::Business: return 6;
        case SeatClass::Economy:  return 9;
        default:                  return 6;
    }
}

int Aircraft::roundSeatsToFullRows(int seats, int seatsPerRow) const
{
    return (seats / seatsPerRow) * seatsPerRow;
}

int Aircraft::assignSeats(int numSeats, SeatClass cls, int startRow)
{
    int row = startRow;
    int seatsPerRowClass = getSeatsPerRowForClass(cls);

    std::vector<char> seatLetters(seatsPerRowClass);
    std::iota(seatLetters.begin(), seatLetters.end(), 'A');

    int price = 0;
    //determine price for this class
    switch(cls)
    {
        case SeatClass::First:    price = 500; break;
        case SeatClass::Business: price = 350; break;
        case SeatClass::Economy:  price = 200; break;
        default:                  price = 200; break;
    }

    for(int i = 0 ; i < numSeats ; i++)
    {
        char col = seatLetters[ i % seatsPerRowClass];
        if(i != 0 && i % seatsPerRowClass == 0) { row++; }

        auto seat = std::make_shared<Seat> (row, col, cls, price, shared_from_this());
        seats.push_back(seat);
    }
    return row + 1;
}

void Aircraft::initializeSeats()
{
    seats.clear();

    //Desired seat counts per class
    int rowFirstSeats = std::ceil(totalSeats * 0.1);           //10% of total seats for First Class
    int rowBusinessSeats = std::ceil(totalSeats * 0.25);      //25* of total seats for Business Class

    // Round First/Business Class Seats to full row 
    int firstSeats = roundSeatsToFullRows( rowFirstSeats, getSeatsPerRowForClass(SeatClass::First));
    int businessSeats = roundSeatsToFullRows( rowBusinessSeats, getSeatsPerRowForClass(SeatClass::Business));

    // Remaining Seats go to Economy
    int economySeats = totalSeats - firstSeats - businessSeats;
    int currentRow = 1;
    currentRow = assignSeats(firstSeats, SeatClass::First , currentRow);
    currentRow = assignSeats(businessSeats, SeatClass::Business, currentRow);
    assignSeats(economySeats, SeatClass::Economy , currentRow);
}

// Getters
const std::vector<std::shared_ptr<Seat>>& Aircraft::getSeats() const
{
    return seats;
}
std::string Aircraft::getModelName() const
{
    return modelName;
}

std::shared_ptr<Seat> Aircraft::getSeatById(const std::string& seatId) const
{
    if(seatId.empty()) {return nullptr; }
    
    for(const auto& seat: seats)
    {
        if(!seat) { continue; }
        if(seat->getSeatID() == seatId)
        {
            return seat;
        }
    }
    return nullptr;
}

// Helper function for flight (sold out or not)
bool Aircraft::hasAvailableSeat() const
{
    for(const auto& seat : seats)
    {
        if(seat->isAvailableSeat()) { return true; }
    }
    return false;
}


// Utilities
// Display seats in a grid filtered by seat class
void Aircraft::displaySeatMap(SeatClass filterClass) const
{
    // Collect all rows for this class
    std::set<int> classRows;
    for (const auto& seat : seats)
        if (seat->getSeatClass() == filterClass)
            classRows.insert(seat->getRow());

    if (classRows.empty()) {
        std::cout << "No seats found for this class.\n";
        return;
    }

    int seatsPerRowClass = getSeatsPerRowForClass(filterClass);
    std::vector<char> seatLetters(seatsPerRowClass);
    std::iota(seatLetters.begin(), seatLetters.end(), 'A');

    const int cellWidth = 4; // consistent width for alignment

    auto printCell = [&](const std::string& symbol) {
        std::cout << " " << symbol;
        int visibleLen = 1;
        int spacesToAdd = cellWidth - 1 - visibleLen;
        for (int i = 0; i < spacesToAdd; ++i)
            std::cout << " ";
    };

    // Header (keep same as before, not adding any new extra space)
    std::cout << std::setw(cellWidth -1 ) << "";
    for (char c : seatLetters)
        std::cout << std::setw(cellWidth) << c;
    std::cout << '\n';

    // Rows
    for (int row : classRows)
    {
        // Row number + one space before grid
        std::cout << std::setw(cellWidth) << row << " ";

        for (char c : seatLetters)
        {
            const Seat* targetSeat = nullptr;
            for (const auto& seat : seats)
            {
                if (seat->getSeatClass() == filterClass &&
                    seat->getRow() == row &&
                    seat->getColumn() == c)
                {
                    targetSeat = seat.get();
                    break;
                }
            }

            std::string symbol = " ";
            if (targetSeat)
            {
                if (targetSeat->isAvailableSeat())
                    symbol = "O";
                else
                    symbol = "\033[31mX\033[0m"; // red X
            }

            printCell(symbol);
        }

        std::cout << '\n';
    }
}






// Helper methods for serialization and serialization (data base dealing)
json Aircraft::toJson() const
{
    json j;
    j["modelName"] = modelName;
    j["totalSeats"] = totalSeats;
    j["seats"] = json::array();
    for (const auto& seat : seats)
    j["seats"].push_back(seat->toJson());
    return j;
}

std::shared_ptr<Aircraft> Aircraft::fromJson(const json& j)
{
    // Extract core attributes
    std::string model = j.at("modelName").get<std::string>();
    int totalSeats = j.at("totalSeats").get<int>();

    // Create instance using your factory method
    auto aircraft = Aircraft::creat(model, totalSeats);

    // Clear any default seats and load from JSON
    aircraft->seats.clear();
    for (const auto& seatJson : j.at("seats")) {
        aircraft->seats.push_back(Seat::fromJson(seatJson));
    }

    return aircraft;
}
