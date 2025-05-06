#include "simulator.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <iostream>
#include <filesystem>  // Add this line
#include <cstring>  // For strncpy

namespace fs = std::filesystem;  // Alias for easier use

namespace Backtesting {

Simulator::Simulator(const std::string& memoryMapName)
    : memoryMapName_(memoryMapName), running_(false) {
}

Simulator::~Simulator() {
    stopSimulation();
}

bool Simulator::initialize() {
    
    std::string tokenMapPath = fs::absolute(TOKEN_MAP_FILE).string();
    std::string contractsDir = fs::absolute(DATA_DIR).string();
    
    std::cout << "Looking for token map at: " << tokenMapPath << std::endl;
    std::cout << "Contracts directory: " << contractsDir << std::endl;

    // If token map doesn't exist, generate it
    if (!fs::exists(tokenMapPath)) {
        std::cout << "Token map not found, generating from contracts..." << std::endl;
        
        // Create data directory if needed
        if (!fs::exists(fs::path(tokenMapPath).parent_path())) {
            fs::create_directory(fs::path(tokenMapPath).parent_path());
        }
        
        if (!tokenMap_.generateFromContracts(contractsDir, tokenMapPath)) {
            std::cerr << "Failed to generate token map from contracts" << std::endl;
            return false;
        }
    }
    
    // Now load the token map
    if (!tokenMap_.loadFromFile(tokenMapPath)) {
        std::cerr << "Failed to load token map from file: " << tokenMapPath << std::endl;
        return false;
    }
    

    if (!memoryMap_.create(memoryMapName_)) {
        std::cerr << "Failed to create memory map: " << memoryMapName_ << std::endl;
        return false;
    }

    return true;
}

void Simulator::runSimulation(const std::string& ticker) {
    int token = tokenMap_.getToken(ticker);
    if (token == -1) {
        std::cerr << "Ticker not found in token map: " << ticker << std::endl;
        return;
    }

    std::vector<ContractData> contractData;
    if (!loadContractData(ticker, contractData)) {
        std::cerr << "Failed to load contract data for ticker: " << ticker << std::endl;
        return;
    }

    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();

    for (const auto& data : contractData) {
        if (!running_) break;

        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_).count();

        // Simulate real-time by waiting until the correct time to send the next tick
        if (elapsed < data.timestamp) {
            std::this_thread::sleep_for(std::chrono::milliseconds(data.timestamp - elapsed));
        }

        updateMemoryMap(data);
    }

    running_ = false;
}

void Simulator::stopSimulation() {
    running_ = false;
}

bool Simulator::loadContractData(const std::string& ticker, std::vector<ContractData>& data) {
    std::string filename = DATA_DIR + ticker + ".csv";
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string timeStr;
        double ltp, bid, ask;

        if (std::getline(iss, timeStr, ',') &&
            iss >> ltp &&
            iss.ignore() &&
            iss >> bid &&
            iss.ignore() &&
            iss >> ask) {
            
            // Convert time string to timestamp (assuming format HH:MM:SS)
            long timestamp = Utils::timeStringToTimestamp(timeStr);

            ContractData cd;
            cd.ltp = ltp;
            cd.bid = bid;
            cd.ask = ask;
            cd.timestamp = timestamp;
            cd.token = tokenMap_.getToken(ticker);
            
            // Get instrument type from token map
            auto tickerInfo = tokenMap_.getMap().at(ticker);
            cd.type = tickerInfo.second;
            
            // // Use std::copy to copy the ticker string
            // std::copy(ticker.begin(), ticker.end(), cd.ticker);
            // cd.ticker[ticker.size()] = '\0';  // Ensure null-termination
            // Replace:
            std::strncpy(cd.ticker, ticker.c_str(), sizeof(cd.ticker) - 1);
            cd.ticker[sizeof(cd.ticker) - 1] = '\0';  // Ensure null termination

            data.push_back(cd);
        }
    }

    return !data.empty();
}


void Simulator::updateMemoryMap(const ContractData& data) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!memoryMap_.writeContractData(data.token, data)) {
        std::cerr << "Failed to write contract data to memory map for token: " << data.token << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "Updated memory map for " << std::string(data.ticker) << " (token: " << data.token << ") in " << duration << " microseconds" << std::endl;
}

bool Simulator::readContractData(const std::string& memoryMapName, int token, ContractData& data) {
    MemoryMap mmap;
    if (!mmap.open(memoryMapName)) {
        return false;
    }

    return mmap.readContractData(token, data);
}

} // namespace Backtesting