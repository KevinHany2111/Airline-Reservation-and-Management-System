#include "IdGenerator.hpp"

std::unordered_set<std::string> IdGenerator::usedIds;

// Helper function to generate random 3-digit strings
std::string IdGenerator::generateThreeDigit() {
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(0, 999);

    int num = dist(rng);
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << num;
    return oss.str();
}

// Generates a unique 3-digit ID string (no duplicates per program run)
std::string IdGenerator::generateId() {
    std::string id;
    do {
        id = generateThreeDigit();
    } while (usedIds.find(id) != usedIds.end());

    usedIds.insert(id);
    return id;
}

// Randomly selects one of the five airline IATA codes
std::string IdGenerator::getRandomIATACode() {
    static std::vector<std::string> codes = { "EM", "AA", "MS", "EK", "QR" };
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<size_t> dist(0, codes.size() - 1);
    return codes[dist(rng)];
}

// Returns a random engineer name for maintnenace log assignment
std::string IdGenerator::getRandomEngineerName() 
{
    static std::vector<std::string> engNames = {
        "James Miller",
        "Olivia Brown",
        "Ethan Johnson",
        "Ava Davis",
        "Noah Wilson"
    };
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<size_t> dist(0, engNames.size() - 1);
    return engNames[dist(rng)];
}


