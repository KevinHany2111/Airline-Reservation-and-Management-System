#ifndef ID_GENERATOR_HPP
#define ID_GENERATOR_HPP

#include <string>
#include <unordered_set>
#include <random>
#include <iomanip>
#include <sstream>
#include <vector>

class IdGenerator {
private:
    // Keeps track of generated IDs during program runtime
    static std::unordered_set<std::string> usedIds;

    // Internal helper: returns a random 3-digit string (000–999)
    static std::string generateThreeDigit();

public:
    // Returns a unique 3-digit numeric string
    static std::string generateId();

    // Returns a random 2-letter IATA airline code
    static std::string getRandomIATACode();

    // Returns a random engineer name for maintnenace log assignment
    static std::string getRandomEngineerName();

};

#endif // ID_GENERATOR_HPP
