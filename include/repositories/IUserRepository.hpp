#ifndef IUSERREPOSITORY_HPP
#define IUSERREPOSITORY_HPP

#include <IRepository.hpp>
#include <optional>

class IUserRepository : public IRepository
{
protected:
    explicit IUserRepository(const std::string& path);
public:
    virtual std::optional<json> findByUsername(const std::string& uname) const = 0 ;
    virtual std::optional<json> authenticate(const std::string& uname, const std::string& pwdHash) const = 0;

    virtual ~IUserRepository() = default;
};

#endif // IUSERREPOSITORY_HPP