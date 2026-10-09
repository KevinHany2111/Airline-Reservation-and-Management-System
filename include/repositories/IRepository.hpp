#ifndef IREPOSITORY_HPP
#define IREPOSITORY_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>
#include "JsonHandler.hpp"

using json = nlohmann::json;

class IRepository
{
protected:
    std::string filePath;
    explicit IRepository(const std::string& path);

public:
    virtual bool add(const json& newObj) = 0;
    virtual bool updateById(const std::string& id, const json& updatedObj) = 0;
    virtual bool removeById(const std::string& id) = 0;
    virtual std::optional<json> findById(const std::string& id) const = 0;
    virtual std::vector<json> getAll() const = 0;

    virtual ~IRepository() = default;
};

#endif // IREPOSITORY_HPP