#include "RepositoryUtils.hpp"
#include "JsonHandler.hpp"
#include <iostream>

std::optional<json> RepositoryUtils::getById(const std::string& filePath, const std::string& idKey, const std::string& idValue)
{
    json data = JsonHandler::loadJson(filePath);
    if (!data.is_array()) return std::nullopt;

    for (const auto& obj : data)
    {
        if (obj.contains(idKey) && obj[idKey] == idValue)
            return obj;
    }
    return std::nullopt;
}

std::vector<json> RepositoryUtils::getAll(const std::string& filePath)
{
    json data = JsonHandler::loadJson(filePath);
    if (!data.is_array()) return {};
    return data.get<std::vector<json>>();
}

bool RepositoryUtils::appendToFile(const std::string& filePath, const json& newObject)
{
    json data = JsonHandler::loadJson(filePath);
    if (!data.is_array()) data = json::array();

    data.push_back(newObject);
    JsonHandler::saveJson(filePath, data);
    return true;
}

bool RepositoryUtils::updateById(const std::string& filePath, const std::string& idKey, const std::string& idValue, const json& updatedObject)
{
    json data = JsonHandler::loadJson(filePath);
    if (!data.is_array()) return false;

    bool updated = false;
    for (auto& obj : data)
    {
        if (obj.contains(idKey) && obj[idKey] == idValue)
        {
            obj = updatedObject;
            updated = true;
            break;
        }
    }
    if (updated) JsonHandler::saveJson(filePath, data);
    return updated;
}

bool RepositoryUtils::removeById(const std::string& filePath, const std::string& idKey, const std::string& idValue)
{
    json data = JsonHandler::loadJson(filePath);
    if (!data.is_array()) return false;

    auto newData = json::array();
    for (const auto& obj : data)
    {
        if (!(obj.contains(idKey) && obj[idKey] == idValue))
            newData.push_back(obj);
    }

    JsonHandler::saveJson(filePath, newData);
    return true;
}
