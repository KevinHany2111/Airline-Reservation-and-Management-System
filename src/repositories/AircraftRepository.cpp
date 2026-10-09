#include "AircraftRepository.hpp"
#include "RepositoryUtils.hpp"

AircraftRepository::AircraftRepository(const std::string& path)
    : IRepository(path) {}


std::optional<json> AircraftRepository::findById(const std::string& id) const
{
    return RepositoryUtils::getById(filePath, "modelName", id);
}

std::vector<json> AircraftRepository::getAll() const
{
    return RepositoryUtils::getAll(filePath);
}

bool AircraftRepository::add(const json& newObj)
{
    return RepositoryUtils::appendToFile(filePath, newObj);
}

bool AircraftRepository::updateById(const std::string& id, const json& updatedObj)
{
    return RepositoryUtils::updateById(filePath, "modelName", id, updatedObj);
}

bool AircraftRepository::removeById(const std::string& id)
{
    return RepositoryUtils::removeById(filePath, "modelName", id);
}
