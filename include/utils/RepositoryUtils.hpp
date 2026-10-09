#ifndef REPOSITORY_UTILS_HPP
#define REPOSITORY_UTILS_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <optional>
#include <vector>

using json = nlohmann::json;

class RepositoryUtils
{
public:
    static std::optional<json> getById(const std::string& filePath, const std::string& idKey, const std::string& idValue);
    static std::vector<json> getAll(const std::string& filePath);
    static bool appendToFile(const std::string& filePath, const json& newObject);
    static bool updateById(const std::string& filePath, const std::string& idKey, const std::string& idValue, const json& updatedObject);
    static bool removeById(const std::string& filePath, const std::string& idKey, const std::string& idValue);
};

#endif // REPOSITORY_UTILS_HPP
