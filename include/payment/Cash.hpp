#ifndef CASH_HPP
#define CASH_HPP

#include "IPayment.hpp"

class Cash : public IPayment
{
public:
    Cash(double amt);
    void pay() const override;
    void proccessRefund() const override;
    void finishRefund() const override;
    
};

#endif // CASH_HPP