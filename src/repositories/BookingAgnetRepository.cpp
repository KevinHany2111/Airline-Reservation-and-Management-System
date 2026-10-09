#include "BookingAgentRepository.hpp"
#include "RepositoryUtils.hpp"

BookingAgentRepository::BookingAgentRepository(const std::string& path)
    : IUserRepository(path)
{
}

std::optional<json> BookingAgentRepository::findById(const std::string& id) const
{
    return RepositoryUtils::getById(filePath, "userId", id);
}

std::vector<json> BookingAgentRepository::getAll() const
{
    return RepositoryUtils::getAll(filePath);
}

bool BookingAgentRepository::add(const json& newObj)
{
    return RepositoryUtils::appendToFile(filePath, newObj);
}

bool BookingAgentRepository::updateById(const std::string& id, const json& updatedObj)
{
    return RepositoryUtils::updateById(filePath, "userId", id, updatedObj);
}

bool BookingAgentRepository::removeById(const std::string& id)
{
    return RepositoryUtils::removeById(filePath, "userId", id);
}

std::optional<json> BookingAgentRepository::findByUsername(const std::string& username) const
{
    return RepositoryUtils::getById(filePath, "username", username);
}

std::optional<json> BookingAgentRepository::authenticate(const std::string& uname, const std::string& pwdHash) const
{
    auto userOpt = findByUsername(uname);
    if (!userOpt) return std::nullopt;

    const json &user = *userOpt;
    if (user.contains("passwordHash") && user["passwordHash"].is_string())
    {
        if (user["passwordHash"].get<std::string>() == pwdHash)
            return user;
    }
    return std::nullopt;
}
