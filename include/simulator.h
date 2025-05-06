#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <string>
#include <vector>
#include <chrono>
#include "token_map.h"
#include "memory_map.h"

namespace Backtesting {

class Simulator {
public:
    Simulator(const std::string& memoryMapName);
    ~Simulator();

    bool initialize();
    void runSimulation(const std::string& ticker);
    void stopSimulation();

    // For reading from another process
    static bool readContractData(const std::string& memoryMapName, int token, ContractData& data);

private:
    bool loadContractData(const std::string& ticker, std::vector<ContractData>& data);
    void updateMemoryMap(const ContractData& data);

    MemoryMap memoryMap_;
    TokenMap tokenMap_;
    std::string memoryMapName_;
    bool running_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
};

} // namespace Backtesting

#endif // SIMULATOR_H