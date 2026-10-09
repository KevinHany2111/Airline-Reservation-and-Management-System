#ifndef PASSENGER_REPOSITORY_HPP
#define PASSENGER_REPOSITORY_HPP

#include "IUserRepository.hpp"

class PassengerRepository : public IUserRepository
{
public:
    explicit PassengerRepository(const std::string& path);

    std::optional<json> findById(const std::string& id) const override;
    std::optional<json> findByUsername(const std::string& username) const override;
    std::optional<json> authenticate(const std::string& uname, const std::string& pwdHash) const override;
    std::vector<json> getAll() const override;
    bool add(const json& newObj) override;
    bool updateById(const std::string& id, const json& updatedObj) override;
    bool removeById(const std::string& id) override;

};

#endif // PASSENGER_REPOSITORY_HPP
