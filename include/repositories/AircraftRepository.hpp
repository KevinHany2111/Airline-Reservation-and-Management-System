#ifndef AIRCRAFTREPOSITORY_HPP
#define AIRCRAFTREPOSITORY_HPP

#include "IRepository.hpp"

class AircraftRepository : public IRepository
{
public:
    explicit AircraftRepository(const std::string& path);

    bool add(const json& newObj) override;
    bool updateById(const std::string& id, const json& updatedObj) override;
    bool removeById(const std::string& id) override;
    std::optional<json> findById(const std::string& id) const override;
    std::vector<json> getAll() const override;
};

#endif // AIRCRAFTREPOSITORY_HPP
