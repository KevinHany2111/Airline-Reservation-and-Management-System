#include <iostream>
#include "Cash.hpp"

// Constructor
Cash::Cash(double amt) : IPayment(amt, Methods::Cash) {}

// Overriding pay() methods
void Cash::pay() const 
{
    std::cout << "Cash payment successful of amount: $" << amount << std::endl;
}

void Cash::proccessRefund() const
{
    std::cout << "Processing refund in cash" << std::endl;
}

void Cash::finishRefund() const
{
    std::cout << "Refund Amount: " << amount << " has been returned to you in cash." << std::endl;
}