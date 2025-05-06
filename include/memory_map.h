#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <string>
#include <sys/mman.h>
#include "constants.h"

namespace Backtesting {

class MemoryMap {
public:
    MemoryMap();
    ~MemoryMap();

    bool create(const std::string& name);
    bool open(const std::string& name);
    void close();

    bool isOpen() const;
    void* getData() const;
    size_t getSize() const;

    bool writeContractData(int token, const ContractData& data);
    bool readContractData(int token, ContractData& data) const;
    // Add this static method declaration to memory_map.h
    static bool readSharedContractData(const std::string& name, int token, ContractData& data);

private:
    std::string name_;
    void* data_;
    size_t size_;
    int fd_;
    bool owner_;
};

} // namespace Backtesting

#endif // MEMORY_MAP_H