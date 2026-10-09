#ifndef IREPORT_HPP
#define IREPORT_HPP

#include <string>
#include <iostream>


class IReport
{
public:
    virtual ~IReport() = default;
    // Generate the report for the given monthYear (format: "MM-YYYY")
    virtual void generate(const std::string& monthYear) = 0;

protected:
    // Helper to save the textual content (body only) to a file (overwrites)
    void saveToTxt(const std::string& filepath, const std::string& content) const;
};

#endif // IREPORT_HPP
