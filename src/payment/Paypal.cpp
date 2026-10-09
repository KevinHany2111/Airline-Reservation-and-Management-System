#include "PayPal.hpp"
#include <iostream>

// Constructor
Paypal::Paypal(double amt, const std::string& e) :
              IPayment(amt, Methods::Paypal) , email(e) {}

// Getter
std::string Paypal::getEmail() const { return email; }

void Paypal::pay() const 
{
    std::cout << "Paypal Paymnet successful of amount: $" << amount << std::endl;
}

void Paypal::proccessRefund() const
{
    std::cout << "processing refund to PayPal account: " << email  << std::endl;
}

void Paypal::finishRefund() const
{
    std::cout << "Refund Amount: " << amount << " has been credited to your PayPal account." << std::endl;
}