#ifndef AUTHENTICATIONMANAGER_HPP
#define AUTHENTICATIONMANAGER_HPP

#include <memory>
#include <string>
#include <optional>
#include "IUser.hpp"
#include "Passenger.hpp"
#include "BookingAgent.hpp"
#include "Administrator.hpp"
#include "AdministratorRepository.hpp"
#include "BookingAgentRepository.hpp"
#include "PassengerRepository.hpp"

class AuthenticationManager
{
private:
    std::shared_ptr<AdministratorRepository> adminRepo;
    std::shared_ptr<BookingAgentRepository> agentRepo;
    std::shared_ptr<PassengerRepository> passengerRepo;

    std::shared_ptr<IUser> currentUser;

public:
    AuthenticationManager(const std::string& adminPath,
                          const std::string& agentPath,
                          const std::string& passengerPath);

    std::shared_ptr<IUser> login(Roles role,
                                 const std::string& username,
                                 const std::string& password);

    std::shared_ptr<IUser> signIn(Roles role,
                              const std::string& username,
                              const std::string& password);

    void logout();
    std::shared_ptr<IUser> getCurrentUser() const;
};

#endif // AUTHENTICATIONMANAGER_HPP
