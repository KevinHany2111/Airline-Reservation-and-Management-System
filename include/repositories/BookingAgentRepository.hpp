#ifndef BOOKINGAGENTREPOSITORY_HPP
#define BOOKINGAGENTREPOSITORY_HPP

#include "IUserRepository.hpp"

class BookingAgentRepository : public IUserRepository
{
public:
    explicit BookingAgentRepository(const std::string& path);

    // IRepository
    std::optional<json> findById(const std::string& id) const override;
    std::vector<json> getAll() const override;
    bool add(const json& newObj) override;
    bool updateById(const std::string& id, const json& updatedObj) override;
    bool removeById(const std::string& id) override;

    // IUserRepository
    std::optional<json> findByUsername(const std::string& username) const override;
    std::optional<json> authenticate(const std::string& uname, const std::string& pwdHash) const override;
};

#endif // BOOKINGAGENTREPOSITORY_HPP
