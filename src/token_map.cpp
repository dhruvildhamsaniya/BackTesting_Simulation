#include "token_map.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>  // Needed for remove_if
#include <dirent.h> // For directory scanning
#include <sys/stat.h>

namespace fs = std::filesystem;

namespace Backtesting {

TokenMap::TokenMap() {
    // Initialize with some default tokens if needed
}

bool TokenMap::generateFromContracts(const std::string& contractsDir, const std::string& outputFile) {
    // Create data directory if it doesn't exist
    mkdir("data", 0777);

    DIR *dir;
    struct dirent *ent;
    int nextToken = 1001;
    
    std::cout << "Scanning contracts directory: " << contractsDir << std::endl;
    
    if ((dir = opendir(contractsDir.c_str())) != nullptr) {
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Failed to create token map file: " << outputFile << std::endl;
            closedir(dir);
            return false;
        }
        
        // Write header
        outFile << "token,ticker,instrument_type\n";
        
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            
            // Skip hidden files and non-CSV files
            if (filename[0] == '.' || filename.size() < 5 || 
                filename.substr(filename.size() - 4) != ".csv") {
                continue;
            }
            
            std::string ticker = filename.substr(0, filename.size() - 4);
            InstrumentType type = InstrumentType::OPTION; // default
            
            // Improved type detection
            if (ticker.find("FUT") != std::string::npos) {
                type = InstrumentType::FUTURE;
            } else if (ticker.find("SPOT") != std::string::npos) {
                type = InstrumentType::SPOT;
            }
            
            std::cout << "Adding contract: " << ticker << " (token: " << nextToken << ")" << std::endl;
            outFile << nextToken++ << "," << ticker << "," << static_cast<int>(type) << "\n";
        }
        closedir(dir);
        outFile.close();
        std::cout << "Successfully generated token map with " << (nextToken - 1001) << " contracts" << std::endl;
        return true;
    } else {
        std::cerr << "Could not open contracts directory: " << contractsDir << std::endl;
        return false;
    }
}

bool TokenMap::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open token map file: " << fs::absolute(filename) << std::endl;
        return false;
    }

    std::string line;
    bool first_line = true;
    int line_num = 0;
    
    while (std::getline(file, line)) {
        line_num++;
        // Skip empty lines
        if (line.empty()) continue;
        
        // Skip header line
        if (first_line) {
            first_line = false;
            continue;
        }

        // Remove any trailing carriage return (for Windows files)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::istringstream iss(line);
        std::string token_str, ticker, type_str;
        
        // Read token,ticker,instrument_type
        if (std::getline(iss, token_str, ',') &&
            std::getline(iss, ticker, ',') &&
            std::getline(iss, type_str)) {
            
            try {
                // Remove any whitespace from values
                token_str.erase(std::remove_if(token_str.begin(), token_str.end(), ::isspace), token_str.end());
                ticker.erase(std::remove_if(ticker.begin(), ticker.end(), ::isspace), ticker.end());
                type_str.erase(std::remove_if(type_str.begin(), type_str.end(), ::isspace), type_str.end());

                int token = std::stoi(token_str);
                InstrumentType type;
                
                if (type_str == "2") type = InstrumentType::OPTION;
                else if (type_str == "1") type = InstrumentType::SPOT;
                else if (type_str == "0") type = InstrumentType::FUTURE;
                else {
                    std::cerr << "Warning: Invalid instrument type '" << type_str << "' at line " << line_num << std::endl;
                    continue;
                }
                
                if (!addMapping(ticker, token, type)) {
                    std::cerr << "Warning: Failed to add mapping for line " << line_num << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing line " << line_num << ": " << e.what() << std::endl;
                continue;
            }
        } else {
            std::cerr << "Warning: Malformed line " << line_num << " in token map file" << std::endl;
        }
    }

    if (tickerToToken_.empty()) {
        std::cerr << "Error: Token map loaded but no valid entries found" << std::endl;
        return false;
    }

    std::cout << "Successfully loaded " << tickerToToken_.size() << " instrument mappings" << std::endl;
    return true;
}

bool TokenMap::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "token,ticker,instrument_type\n"; // Write header
    for (const auto& [ticker, pair] : tickerToToken_) {
        const auto& [token, type] = pair;
        std::string typeStr;
        switch (type) {
            case InstrumentType::FUTURE: typeStr = "0"; break;
            case InstrumentType::SPOT: typeStr = "1"; break;
            case InstrumentType::OPTION: typeStr = "2"; break;
        }
        file << token << "," << ticker << "," << typeStr << "\n";
    }

    return true;
}

int TokenMap::getToken(const std::string& ticker) const {
    auto it = tickerToToken_.find(ticker);
    if (it != tickerToToken_.end()) {
        return it->second.first;
    }
    return -1; // Invalid token
}

std::string TokenMap::getTicker(int token) const {
    auto it = tokenToTicker_.find(token);
    if (it != tokenToTicker_.end()) {
        return it->second.first;
    }
    return "";
}

bool TokenMap::addMapping(const std::string& ticker, int token, InstrumentType type) {
    if (ticker.empty() || token < 0) {
        return false;
    }

    tickerToToken_[ticker] = {token, type};
    tokenToTicker_[token] = {ticker, type};
    return true;
}

const std::unordered_map<std::string, std::pair<int, InstrumentType>>& TokenMap::getMap() const {
    return tickerToToken_;
}

} // namespace Backtesting