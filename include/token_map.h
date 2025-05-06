#ifndef TOKEN_MAP_H
#define TOKEN_MAP_H

#include <string>
#include <unordered_map>
#include "constants.h"

namespace Backtesting {

class TokenMap {
public:
    TokenMap();
    ~TokenMap() = default;

    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;

    int getToken(const std::string& ticker) const;
    std::string getTicker(int token) const;
    bool addMapping(const std::string& ticker, int token, InstrumentType type);

    const std::unordered_map<std::string, std::pair<int, InstrumentType>>& getMap() const;
    // Add this method declaration in token_map.h
    
    bool generateFromContracts(const std::string& contractsDir, const std::string& outputFile);

private:
    std::unordered_map<std::string, std::pair<int, InstrumentType>> tickerToToken_;
    std::unordered_map<int, std::pair<std::string, InstrumentType>> tokenToTicker_;
};

} // namespace Backtesting

#endif // TOKEN_MAP_H