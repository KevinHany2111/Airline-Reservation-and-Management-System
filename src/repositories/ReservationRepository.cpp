#include "ReservationRepository.hpp"
#include "RepositoryUtils.hpp"
#include <iostream>
#include <algorithm>

ReservationRepository::ReservationRepository(const std::string& path) : IRepository(path) {}

std::optional<json> ReservationRepository::findById(const std::string& id) const
{
    return RepositoryUtils::getById(filePath, "reservationId", id);
}

std::vector<json> ReservationRepository::getAll() const
{
    return RepositoryUtils::getAll(filePath);
}

bool ReservationRepository::add(const json& newObj)
{
    return RepositoryUtils::appendToFile(filePath, newObj);
}

bool ReservationRepository::updateById(const std::string& id, const json& updatedObj)
{
    return RepositoryUtils::updateById(filePath, "reservationId", id, updatedObj);
}

bool ReservationRepository::removeById(const std::string& id)
{
    return RepositoryUtils::removeById(filePath, "reservationId", id);
}


