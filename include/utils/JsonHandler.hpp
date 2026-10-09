#ifndef JSONHANDLER_HPP
#define JSONHANDLER_HPP

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonHandler
{
public:
    static json loadJson(const std::string& filePath);
    static void saveJson(const std::string& filePath, const json& data);
    static bool fileExists(const std::string& filePath);
    static bool validateJson(const json& data, const std::vector<std::string>& requiredFields);

    static json readArray(const std::string& filePath);
    static bool appendToFile(const std::string& filePath, const json& newObject);
    static bool removeById(const std::string& filePath,
                           const std::string& idField,
                           const std::string& idValue);
    static bool updateById(const std::string& filePath,
                           const std::string& idField,
                           const std::string& idValue,
                           const json& updateObject);

};


#endif //JSONHANDLER_HPP