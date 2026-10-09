#include "IPayment.hpp"

// Constructor
IPayment::IPayment(double amt, Methods m) : amount(amt), method(m) {}

// Getters
double IPayment::getAmount() const { return amount; }
Methods IPayment::getMethod() const { return method; }
std::string IPayment::getMethodStr() const 
{
    switch(method)
    {
        case Methods::Cash:       return "Cash";
        case Methods::CreditCard: return "CreditCard";
        case Methods::Paypal:     return "Paypal";
        default:                  return "Unknown";
    }
}

// Private method for ReservationService
void IPayment::setAmount(double amt) { amount = amt; }
