#include <iostream>
#include <string>
#include <filesystem>
#include "simulator.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <ticker>" << std::endl;
        std::cerr << "Example: " << argv[0] << " NIFTY02JAN2521800PE.NFO" << std::endl;
        return 1;
    }

    std::string ticker = argv[1];
    std::string memoryMapName = "/backtesting_mmap";

    // Verify data directory exists
    if (!fs::exists("data")) {
        std::cerr << "Error: 'data' directory not found. Please create it in the same directory as the executable." << std::endl;
        std::cerr << "Expected path: " << fs::absolute("data") << std::endl;
        return 1;
    }

    // Verify token map file exists
    std::string tokenMapPath = "data/token_map.csv";
    if (!fs::exists(tokenMapPath)) {
        std::cerr << "Error: token_map.csv not found at: " << fs::absolute(tokenMapPath) << std::endl;
        std::cerr << "Please create a token_map.csv file with format: ticker,token,type" << std::endl;
        return 1;
    }

    // Verify contract file exists
    std::string contractFile = "data/contracts/" + ticker + ".csv";
    if (!fs::exists(contractFile)) {
        std::cerr << "Error: Contract file not found: " << fs::absolute(contractFile) << std::endl;
        std::cerr << "Please ensure the contract data file exists in data/contracts/" << std::endl;
        return 1;
    }

    Backtesting::Simulator simulator(memoryMapName);
    if (!simulator.initialize()) {
        std::cerr << "Failed to initialize simulator" << std::endl;
        return 1;
    }

    // Debug: Print the token map loading status
    std::cout << "Starting simulation for ticker: " << ticker << std::endl;
    simulator.runSimulation(ticker);
    std::cout << "Simulation completed" << std::endl;

    return 0;
}