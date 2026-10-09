#include "MaintenanceLog.hpp"
#include <iostream>

MaintenanceLog::MaintenanceLog(const std::string& craftName, const std::string& descr ) :
                               logId( "LOG" + IdGenerator::generateId()),
                               aircraftName(craftName), description(descr),
                               engineerName("Eng/ " + IdGenerator::getRandomEngineerName()) {}

// Getters
std::string MaintenanceLog::getLogId() const { return logId; }
std::string MaintenanceLog::getAircraftName() const { return aircraftName; }
std::string MaintenanceLog::gatDescription() const { return description; }
std::string MaintenanceLog::getEngineerName() const { return engineerName; }

// Serialization
json MaintenanceLog::toJson() const
{
    return {
        {"logId", logId},
        {"aircraftModel", aircraftName},
        {"description", description},
        {"engineerName", engineerName}
    };
}

MaintenanceLog MaintenanceLog::fromJson(const json& j)
{
    MaintenanceLog log(
        j.at("aircraftModel").get<std::string>(),
        j.at("description").get<std::string>()
    );

    // Overwrite generated values if stored ones exist
    if (j.contains("logId")) { log.logId = j.at("logId"); }
    if (j.contains("engineerName")) { log.engineerName = j.at("engineerName"); }

    return log;
}
