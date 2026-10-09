#include "IReport.hpp"
#include <fstream>
#include <iostream>

void IReport::saveToTxt(const std::string& filepath, const std::string& content) const
{
    std::ofstream ofs(filepath, std::ios::trunc);
    if (!ofs.is_open())
    {
        std::cerr << "[IReport] Error: Unable to write report to " << filepath << std::endl;
        return;
    }
    ofs << content;
    ofs.close();
}
