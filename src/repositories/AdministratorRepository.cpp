#include "AdministratorRepository.hpp"
#include "RepositoryUtils.hpp"

AdministratorRepository::AdministratorRepository(const std::string& path)
    : IUserRepository(path) {}

std::optional<json> AdministratorRepository::findById(const std::string& id) const
{
    return RepositoryUtils::getById(filePath, "userId", id);
}

std::vector<json> AdministratorRepository::getAll() const
{
    return RepositoryUtils::getAll(filePath);
}

bool AdministratorRepository::add(const json& newObj)
{
    return RepositoryUtils::appendToFile(filePath, newObj);
}

bool AdministratorRepository::updateById(const std::string& id, const json& updatedObj)
{
    return RepositoryUtils::updateById(filePath, "userId", id, updatedObj);
}

bool AdministratorRepository::removeById(const std::string& id)
{
    return RepositoryUtils::removeById(filePath, "usreId", id);
}

std::optional<json> AdministratorRepository::findByUsername(const std::string& username) const
{
    return RepositoryUtils::getById(filePath, "username", username);
}

std::optional<json> AdministratorRepository::authenticate(const std::string& uname, const std::string& pwdHash) const
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
