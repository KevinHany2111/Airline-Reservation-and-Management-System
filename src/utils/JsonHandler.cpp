#include <iostream>
#include <fstream>
#include <filesystem>
#include "JsonHandler.hpp"
/*
json JsonHandler::loadJson(const std::string& filePath)
{
    if(!fileExists(filePath))
    {
        std::cerr << "[JsonHandler] File not found: " << filePath << std::endl;
        return json::object();
    }

    std::ifstream file(filePath);
    
    if(!file.is_open())
    {
        std::cerr << "[JsonHandler] Error opening file: " << filePath << std::endl;
        return json::object();
    }

    try
    {
        json data;
        file >> data;
        return data;
    }
    catch(const json::parse_error& e)
    {
        std::cerr << "[JsonHandler] Error: Invalid JSON in file: " << filePath << "\n"
                  << "Details: " << e.what() << std::endl;
        return json::object();
    }
    catch(const std::exception& e)
    {
        std::cerr << "[JsonHandler] Unexpected error reading JSON file: " << e.what() << std::endl;
        return json::object();
    }
    
}
*/
json JsonHandler::loadJson(const std::string& filePath)
{
    if (!fileExists(filePath))
    {
        // Create an empty file initialized as array
        std::ofstream(filePath) << "[]";
        return json::array();
    }

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "[JsonHandler] Error opening file: " << filePath << std::endl;
        return json::array();
    }

    // Check if file is empty before parsing
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        // Initialize as empty JSON array for collection-type files
        return json::array();
    }

    try
    {
        json data;
        file >> data;
        return data;
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "[JsonHandler] Warning: Invalid or empty JSON file: " << filePath
                  << ". Resetting to empty array." << std::endl;
        return json::array();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[JsonHandler] Unexpected error reading JSON file: "
                  << e.what() << std::endl;
        return json::array();
    }
}


void JsonHandler::saveJson(const std::string& filePath, const json& data)
{
    try
    {
        std::ofstream file(filePath);
        if(!file.is_open())
        {
            std::cerr << "[JsonHandler] Error: Unable to open file for writing: " << filePath << std::endl;
            return;
        }
        file << std::setw(4) << data << std::endl;
        //std::cout << "[JsonHandler] Successfully saved JSON to " << filePath << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << "[JsonHandler] Error writing to file " << filePath << ": " << e.what() << std::endl;
    }
}

bool JsonHandler::fileExists(const std::string& filePath)
{
    return std::filesystem::exists(filePath);
}

bool JsonHandler::validateJson(const json& data, const std::vector<std::string>& requiredFields)
{
    if(!data.is_object())
    {
        std::cerr << "[JsonHandler] Invalid JSON: not an object." << std::endl;
        return false;
    }
    for(const auto& field : requiredFields )
    {
        if(!data.contains(field))
        {
            std::cerr << "[JsonHandler] Missing required field: " << field << std::endl;
            return false;
        }
    }
    return true;
}



// Read JSON array 
json JsonHandler::readArray(const std::string& filePath)
{
    json data = loadJson(filePath);

    if (!data.is_array())
    {
        // If file is empty or invalid, start with an empty array
        std::cout << "[JsonHandler] File " << filePath << " not an array. Creating empty array." << std::endl;
        return json::array();
    }

    return data;
}

// Append object to JSON array file
bool JsonHandler::appendToFile(const std::string& filePath, const json& newObject)
{
    try
    {
        json data = readArray(filePath);
        data.push_back(newObject);
        saveJson(filePath, data);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[JsonHandler] appendToFile failed for " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}

// Remove object by ID field
bool JsonHandler::removeById(const std::string& filePath,
                             const std::string& idField,
                             const std::string& idValue)
{
    try
    {
        json data = readArray(filePath);
        bool removed = false;

        auto it = std::remove_if(data.begin(), data.end(),
            [&](const json& obj)
            {
                return obj.contains(idField) && obj[idField] == idValue;
            });

        if (it != data.end())
        {
            data.erase(it, data.end());
            removed = true;
        }

        if (removed)
        {
            saveJson(filePath, data);
            //std::cout << "[JsonHandler] Removed entry with " << idField << " = " << idValue << " from " << filePath << std::endl;
            return true;
        }

        std::cerr << "[JsonHandler] No entry found with " << idField << " = " << idValue << " in " << filePath << std::endl;
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[JsonHandler] removeById failed: " << e.what() << std::endl;
        return false;
    }
}

// Update object by ID
bool JsonHandler::updateById(const std::string& filePath,
                             const std::string& idField,
                             const std::string& idValue,
                             const json& updatedObject)
{
    try
    {
        json data = readArray(filePath);
        bool updated = false;

        for (auto& obj : data)
        {
            if (obj.contains(idField) && obj[idField] == idValue)
            {
                // Replace old data with updated version
                obj = updatedObject;
                updated = true;
                break;
            }
        }

        if (updated)
        {
            saveJson(filePath, data);
            std::cout << "[JsonHandler] Updated entry with " << idField << " = " << idValue << " in " << filePath << std::endl;
            return true;
        }

        std::cerr << "[JsonHandler] No entry found to update for " << idField << " = " << idValue << std::endl;
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[JsonHandler] updateById failed: " << e.what() << std::endl;
        return false;
    }
}
