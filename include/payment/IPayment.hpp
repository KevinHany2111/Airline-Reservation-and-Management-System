#ifndef IPAYMENT_HPP
#define IPAYMENT_HPP

#include <string>

enum class Methods { Cash, CreditCard, Paypal };

class IPayment
{
protected:
    double amount;
    Methods method;

    void setAmount(double amt);
    
public:
    IPayment(double amt, Methods m);

    //Getters
    double getAmount() const;
    Methods getMethod() const;
    std::string getMethodStr() const;

    // Pure Virtual function
    virtual void pay() const = 0;
    virtual void proccessRefund() const = 0;
    virtual void finishRefund() const = 0;

    virtual ~IPayment() = default;
};



#endif // IPAYMENTMETHOD_HPP