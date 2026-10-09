#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cctype>
#include <utility>
#include "Seat.hpp"
//#include "Validator.hpp"
//#include "Aircraft.hpp"

//Constructors
Seat::Seat(int r, char c, SeatClass sc, int price ,const std::shared_ptr<Aircraft>& parent) :
 row(r) , column(c) , Class(sc) , seatPrice(price) , isAvailable(true), aircraft(parent) {}

//Getters
int Seat::getRow() const { return row; }
char Seat::getColumn() const { return column; }
SeatClass Seat::getSeatClass() const { return Class; }
std::string Seat::getSeatClassName() const
{
    switch (Class)
    {
    case SeatClass::First :    return "First";
    case SeatClass::Business : return "Business";
    case SeatClass::Economy :  return "Economy";
    default:                   return "Unknown";
    }
}
int Seat::getSeatPrice() const {return seatPrice; }
bool Seat::isAvailableSeat() const { return isAvailable; }
std::string Seat::getSeatID() const 
{
    if( row <= 0) { return "?"; }
    return std::to_string(row) + column;
}
//Setters
void Seat::setRow(int r) { row = r; }
void Seat::setColumn(char c) { column = c; }
void Seat::setSeatClass(SeatClass cls) { Class = cls; }


void Seat::setAvailability(bool availability) { isAvailable = availability; }


std::pair<int, char> Seat::parseSeatId(const std::string& seatId)
{
    std::string digits;
    std::string letters;
    for (char ch : seatId) {
        if (std::isdigit(static_cast<unsigned char>(ch)))
            digits.push_back(ch);
        else if (std::isalpha(static_cast<unsigned char>(ch)))
            letters.push_back(ch);
    }

    int row = 0;
    if (!digits.empty()) {
        try {
            row = std::stoi(digits);
        } catch (...) {
            row = 0; // fallback safe value, but ideally validate earlier
        }
    }

    char column = 'A';
    if (!letters.empty()) {
        // take the first letter (uppercase)
        column = static_cast<char>(std::toupper(static_cast<unsigned char>(letters[0])));
    }

    return {row, column};
}


// Utilities
void Seat::Display() const 
{
    std::cout << "Not yet implemented" << std::endl;
    //see what is your wanted form (i think it is needed after reservation from passenger or a booking agent)
}

// Converters
std::string Seat::seatClassToString(SeatClass cls)
{
    switch (cls)
    {
        case SeatClass::First:    return "First";
        case SeatClass::Business: return "Business";
        case SeatClass::Economy:  return "Economy";
        default:                  return "Unknown";
    }
}
SeatClass Seat::seatClassFromString(const std::string& str)
{
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "first")    return SeatClass::First;
    if (lower == "business") return SeatClass::Business;
    if (lower == "economy")  return SeatClass::Economy;

    throw std::invalid_argument("Invalid SeatClass string: " + str);
}

// Helper methods for serialization and serialization (data base dealing)
json Seat::toJson() const {
    json j;
    j["seatId"] = getSeatID();            // keep identity
    j["seatClass"] = getSeatClassName(); 
    j["price"] = getSeatPrice();
    j["isAvailable"] = isAvailable;
    return j;
} 
std::shared_ptr<Seat> Seat::fromJson(const json& j)
{
    auto seat = std::make_shared<Seat>();

    // Derive row and column from seatId
    auto [row, column] = Seat::parseSeatId(j.at("seatId").get<std::string>());
    seat->row = row;
    seat->column = column;

    // Basic info
    seat->Class = seatClassFromString(j.at("seatClass").get<std::string>());
    seat->seatPrice = j.at("price").get<double>();
    seat->isAvailable = j.at("isAvailable").get<bool>();

    return seat;
}


/*
Seat Seat::fromJson(const json& j)
{
    SeatClass cls = seatClassFromString(j.at("class"));
    Seat seat(
        j.at("row"),
        j.at("column").get<std::string>()[0],
        cls,
        j.at("price"),
        nullptr // aircraft will be assigned externally
    );
    seat.isAvailable = j.value("available", true);
    return seat;
}


std::shared_ptr<Seat> Seat::fromJson(const json& j)
{
    SeatClass cls = seatClassFromString(j.at("class"));

    auto seat = std::make_shared<Seat>(
        j.at("row"),
        j.at("column").get<std::string>()[0],
        cls,
        j.at("price"),
        nullptr // aircraft will be assigned externally
    );

    seat->isAvailable = j.value("available", true);
    return seat;
}
*/



/*
void Seat::setRow(int r) 
{

    if(auto parent = aircraft.lock())
    {
        if(Validator::isValidSeatRow(r, parent))
        {
            row = r;
        }
        else
        {
            cout << "Invalid row number for this aircraft" << endl;
        }
    }
    else
    {
        cout << "//error message to debug in case of lock() failure " << endl;
    }

}
void Seat::setColumn(char c)
{

    if(auto parent = aircraft.lock())
    {
        if(Validator::isValidSeatColumn(c, parent))
        {
            column = c;
        }
        else
        {
            cout << "Invalid column number for this aircraft" << endl;
        }
    }
    else
    {
        cout << "//error message to debug in case of lock() failure" << endl;
    }

}
*/