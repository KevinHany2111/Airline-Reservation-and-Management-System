#include "PassengerRepository.hpp"
#include "RepositoryUtils.hpp"
#include <iostream>

PassengerRepository::PassengerRepository(const std::string& path)
    : IUserRepository(path)
{
}

// IRepository implementations
std::optional<json> PassengerRepository::findById(const std::string& id) const
{
    return RepositoryUtils::getById(filePath, "userId", id);
}

std::vector<json> PassengerRepository::getAll() const
{
    return RepositoryUtils::getAll(filePath);
}

bool PassengerRepository::add(const json& newObj)
{
    return RepositoryUtils::appendToFile(filePath, newObj);
}

bool PassengerRepository::updateById(const std::string& id, const json& updatedObj)
{
    return RepositoryUtils::updateById(filePath, "userId", id, updatedObj);
}

bool PassengerRepository::removeById(const std::string& id)
{
    return RepositoryUtils::removeById(filePath, "userId", id);
}

// IUserRepository implementations
std::optional<json> PassengerRepository::findByUsername(const std::string& username) const
{
    return RepositoryUtils::getById(filePath, "username", username);
}

std::optional<json> PassengerRepository::authenticate(const std::string& uname, const std::string& pwdHash) const
{
    auto userOpt = findByUsername(uname);
    if (!userOpt) return std::nullopt;

    const json &user = *userOpt;
    // expecting stored hashed password under key "password"
    if (user.contains("passwordHash") && user["passwordHash"].is_string())
    {
        if (user["passwordHash"].get<std::string>() == pwdHash)
            return user;
    }
    return std::nullopt;
}
