#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace Backtesting {

class Utils {
public:
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static long timeStringToTimestamp(const std::string& timeStr);
    static std::string timestampToTimeString(long timestamp);
    static bool fileExists(const std::string& path);
};

} // namespace Backtesting

#endif // UTILS_H