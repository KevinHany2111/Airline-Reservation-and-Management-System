#ifndef MAINTENANCEREPOSITORY_HPP
#define MAINTENANCEREPOSITORY_HPP

#include "IRepository.hpp"

class MaintenanceRepository : public IRepository
{
public:
    explicit MaintenanceRepository(const std::string& path);

    bool add(const json& newObj) override;
    bool updateById(const std::string& id, const json& updatedObj) override;
    bool removeById(const std::string& id) override;
    std::optional<json> findById(const std::string& id) const override;
    std::vector<json> getAll() const override;

};

#endif // MAINTENANCEREPOSITORY_HPP