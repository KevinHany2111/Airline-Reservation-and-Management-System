#ifndef FLIGHTREPOSITORY_HPP
#define FLIGHTREPOSITORY_HPP

#include "IRepository.hpp"
#include "Flight.hpp"
#include <memory>

class FlightRepository : public IRepository
{
public:
    explicit FlightRepository(const std::string& path);

    bool add(const json& newObj) override;
    bool updateById(const std::string& id, const json& updatedObj) override;
    bool removeById(const std::string& id) override;
    std::optional<json> findById(const std::string& id) const override;
    std::vector<json> getAll() const override;
    std::shared_ptr<Flight> getById(const std::string& flightNumber);

    bool saveFlight(const Flight& flight);

    std::vector<std::shared_ptr<Flight>> findFlights(const std::string& origin,
                                                     const std::string& destination,
                                                     const std::string& departureDate) const;

    static std::string nextDateString(const std::string& dateYMD);

};

#endif // FLIGHTREPOSITORY_HPP