#ifndef CREDITCARD_HPP
#define CREDITCARD_HPP

#include "IPayment.hpp"
#include <string>

class CreditCard : public IPayment
{
private:
    std::string cardNumber;

public:
    CreditCard(double amt, const std::string& cardNum);

    // Getter
    std::string getCardNumber() const;

    void pay() const override;
    void proccessRefund() const override;
    void finishRefund() const override;

};

#endif // CREDITCARD_HPP