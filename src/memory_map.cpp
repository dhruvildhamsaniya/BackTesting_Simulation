#include "memory_map.h"
#include "utils.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <stdexcept>

namespace Backtesting {

MemoryMap::MemoryMap() 
    : data_(nullptr), size_(0), fd_(-1), owner_(false) {
}

MemoryMap::~MemoryMap() {
    close();
}

bool MemoryMap::create(const std::string& name) {
    if (isOpen()) {
        close();
    }

    name_ = name;
    fd_ = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd_ == -1) {
        return false;
    }

    if (ftruncate(fd_, MEMORY_MAP_SIZE) == -1) {
        close();
        return false;
    }

    data_ = mmap(nullptr, MEMORY_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (data_ == MAP_FAILED) {
        close();
        return false;
    }

    size_ = MEMORY_MAP_SIZE;
    owner_ = true;

    // Initialize memory
    std::memset(data_, 0, size_);
    return true;
}

bool MemoryMap::open(const std::string& name) {
    if (isOpen()) {
        close();
    }

    name_ = name;
    fd_ = shm_open(name.c_str(), O_RDWR, 0666);
    if (fd_ == -1) {
        return false;
    }

    struct stat st;
    if (fstat(fd_, &st) == -1) {
        close();
        return false;
    }

    size_ = st.st_size;
    data_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (data_ == MAP_FAILED) {
        close();
        return false;
    }

    owner_ = false;
    return true;
}

void MemoryMap::close() {
    if (data_ != nullptr && data_ != MAP_FAILED) {
        munmap(data_, size_);
        data_ = nullptr;
    }

    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }

    if (owner_ && !name_.empty()) {
        shm_unlink(name_.c_str());
    }

    size_ = 0;
    owner_ = false;
}

bool MemoryMap::isOpen() const {
    return data_ != nullptr && data_ != MAP_FAILED;
}

void* MemoryMap::getData() const {
    return data_;
}

size_t MemoryMap::getSize() const {
    return size_;
}

// Add this implementation to memory_map.cpp
bool MemoryMap::readSharedContractData(const std::string& name, int token, ContractData& data) {
    MemoryMap mmap;
    if (!mmap.open(name)) {
        return false;
    }
    return mmap.readContractData(token, data);
}

bool MemoryMap::writeContractData(int token, const ContractData& data) {
    if (!isOpen()) {
        return false;
    }

    // Calculate offset based on token
    size_t offset = (token % (size_ / CONTRACT_DATA_SIZE)) * CONTRACT_DATA_SIZE;
    if (offset + sizeof(ContractData) > size_) {
        return false;
    }

    std::memcpy(static_cast<char*>(data_) + offset, &data, sizeof(ContractData));
    return true;
}

bool MemoryMap::readContractData(int token, ContractData& data) const {
    if (!isOpen()) {
        return false;
    }

    size_t offset = (token % (size_ / CONTRACT_DATA_SIZE)) * CONTRACT_DATA_SIZE;
    if (offset + sizeof(ContractData) > size_) {
        return false;
    }

    std::memcpy(&data, static_cast<char*>(data_) + offset, sizeof(ContractData));
    return true;
}

} // namespace Backtesting