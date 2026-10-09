#ifndef PAYPAL_HPP
#define PAYPAL_HPP

#include "IPayment.hpp"
#include <string>

class Paypal : public IPayment
{
private:
    std::string email;
public:
    Paypal(double amt, const std::string& e);

    // Getter
    std::string getEmail() const;

    void pay() const override;
    void proccessRefund() const override;
    void finishRefund() const override;
};

#endif // PAYPAL_HPP