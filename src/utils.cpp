#include "utils.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <sys/stat.h>

namespace Backtesting {

std::vector<std::string> Utils::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

long Utils::timeStringToTimestamp(const std::string& timeStr) {
    auto parts = splitString(timeStr, ':');
    if (parts.size() != 3) {
        return 0;
    }

    int hours = std::stoi(parts[0]);
    int minutes = std::stoi(parts[1]);
    int seconds = std::stoi(parts[2]);

    return hours * 3600 + minutes * 60 + seconds;
}

std::string Utils::timestampToTimeString(long timestamp) {
    int hours = timestamp / 3600;
    int remaining = timestamp % 3600;
    int minutes = remaining / 60;
    int seconds = remaining % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;
    
    return oss.str();
}

bool Utils::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

} // namespace Backtesting