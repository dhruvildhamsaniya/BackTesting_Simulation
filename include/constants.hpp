#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include <cstdint>

const size_t BLOCK_SIZE = 32;

enum InstrumentType {
    SPOT = 0,
    FUTURE = 1,
    OPTION = 2
};

struct ContractData {
    float ltp;
    float bid;
    float ask;
    uint64_t timestamp;  // seconds from 9:15:00
};

#endif
