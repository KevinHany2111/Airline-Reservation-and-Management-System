#ifndef RESERVATIONREPOSITORY_HPP
#define RESERVATIONREPOSITORY_HPP

#include "IRepository.hpp"

class ReservationRepository : public IRepository
{
public:
    explicit ReservationRepository(const std::string& path);

    bool add(const json& newObj) override;
    bool updateById(const std::string& id, const json& updatedObj) override;
    bool removeById(const std::string& id) override;
    std::optional<json> findById(const std::string& id) const override;
    std::vector<json> getAll() const override;
};

#endif // RESERVATIONREPOSITORY_HPP
