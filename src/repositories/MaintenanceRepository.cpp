#include "MaintenanceRepository.hpp"
#include "RepositoryUtils.hpp"


MaintenanceRepository::MaintenanceRepository(const std::string& path)
    : IRepository(path) {}


bool MaintenanceRepository::add(const json& newObj)
{
    return RepositoryUtils::appendToFile(filePath, newObj);
}

bool MaintenanceRepository::updateById(const std::string& id, const json& updatedObj)
{
    return RepositoryUtils::updateById(filePath, "logId", id, updatedObj);
}

bool MaintenanceRepository::removeById(const std::string& id)
{
    return RepositoryUtils::removeById(filePath, "logId", id);
}

std::optional<json> MaintenanceRepository::findById(const std::string& id) const
{
    return RepositoryUtils::getById(filePath, "logId", id);
}

std::vector<json> MaintenanceRepository::getAll() const
{
    return RepositoryUtils::getAll(filePath);
}
