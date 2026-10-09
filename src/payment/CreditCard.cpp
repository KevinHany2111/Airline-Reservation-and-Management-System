#include <iostream>
#include "CreditCard.hpp"

// Constructor
CreditCard::CreditCard(double amt, const std::string& cardNum) :
                       IPayment(amt, Methods::CreditCard) , cardNumber(cardNum) {}

// Getter
std::string CreditCard::getCardNumber() const { return cardNumber; }

// Overriding pay() method
void CreditCard::pay() const 
{
    std::cout << "Credit Card Payment Successful of amount: $" << amount << std::endl;
}

void CreditCard::proccessRefund() const
{
    std::cout << "processing refund to credit card: " << cardNumber << std::endl;
}

void CreditCard::finishRefund() const
{
    std::cout << "Refund Amount: " << amount << " has been credited to your credit card account." << std::endl;
}