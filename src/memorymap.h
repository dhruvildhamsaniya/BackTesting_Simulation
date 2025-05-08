#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include <cstddef>
#include <string>
#include <cstdint>
#include "constants.hpp"  // <-- use this instead of redefining ContractData

class MemoryMap {
public:
    static constexpr size_t BLOCK_SIZE = sizeof(ContractData);

    MemoryMap(size_t contractCount);
    ~MemoryMap();

    void write(size_t index, const ContractData& cd);
    void dump_all(size_t contractCount) const;

private:
    std::string mmap_file;
    int fd;
    uint8_t* data;
    size_t total_size;
};

#endif // MEMORYMAP_H
