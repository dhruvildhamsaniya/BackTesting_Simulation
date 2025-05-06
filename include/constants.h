#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace Backtesting {

const std::string DATA_DIR = "data/contracts/";
const std::string TOKEN_MAP_FILE = "data/token_map.csv";

const size_t MEMORY_MAP_SIZE = 1024 * 1024; // 1MB memory map
const size_t CONTRACT_DATA_SIZE = 32; // 32 bytes per contract

// Instrument types
enum class InstrumentType {
    FUTURE = 0,
    SPOT = 1,
    OPTION = 2
};

#pragma pack(push, 1)
struct ContractData {
    double ltp;
    double bid;
    double ask;
    long timestamp;
    InstrumentType type;
    int token;
    char ticker[32]; // Fixed size for ticker name
};
#pragma pack(pop)

} // namespace Backtesting

#endif // CONSTANTS_H