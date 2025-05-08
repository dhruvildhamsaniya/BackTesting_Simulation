#include "constants.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <cstring>  // <-- required for std::memcpy
#include <stdexcept>
#include "memorymap.h"


MemoryMap::MemoryMap(size_t contractCount) {
    total_size = contractCount * BLOCK_SIZE;
    mmap_file = "contract_data.mmap";

    fd = open(mmap_file.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) throw std::runtime_error("Failed to open mmap file");

    if (lseek(fd, total_size - 1, SEEK_SET) == -1)
        throw std::runtime_error("Failed to stretch file");

    if (::write(fd, "", 1) != 1)
        throw std::runtime_error("Failed to write last byte");

    data = reinterpret_cast<uint8_t*>(
        mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)
    );
    if (data == MAP_FAILED)
        throw std::runtime_error("mmap failed");
}

MemoryMap::~MemoryMap() {
    munmap(data, total_size);
    close(fd);
}

void MemoryMap::write(size_t index, const ContractData& cd) {
    if (index * BLOCK_SIZE >= total_size)
        throw std::out_of_range("Index out of range in mmap write");

    std::memcpy(data + index * BLOCK_SIZE, &cd, BLOCK_SIZE);
}

void MemoryMap::dump_all(size_t contractCount) const {
    for (size_t i = 0; i < contractCount; ++i) {
        const ContractData* cd = reinterpret_cast<const ContractData*>(data + i * BLOCK_SIZE);
        std::cout << "Index " << i
                  << ": LTP=" << cd->ltp
                  << ", Bid=" << cd->bid
                  << ", Ask=" << cd->ask
                  << ", Timestamp=" << cd->timestamp << "\n";
    }
}