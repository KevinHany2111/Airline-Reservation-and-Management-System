#ifndef MAINTENANCELOG_HPP
#define MAINTENANCELOG_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "IdGenerator.hpp"

using json = nlohmann::json;


class MaintenanceLog
{
private:
    std::string logId;
    std::string aircraftName;
    std::string description;
    std::string engineerName;

public:
    MaintenanceLog(const std::string& craftName,const std::string& descr );

    // Getters
    std::string getLogId() const;
    std::string getAircraftName() const;
    std::string gatDescription() const;
    std::string getEngineerName() const;

    // Serialization
    json toJson() const;
    static MaintenanceLog fromJson(const json& j);

};

#endif // MAINTENANCELOG_HPP