#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <unordered_map>
#include <stdexcept>
#include <string>
#include <cstdlib>

class MemoryManager {
private:
    std::unordered_map<int, std::pair<void*, size_t>> memory;
    int nextAddress = 0;
    size_t memorySize;
    char* memoryBlock;

    MemoryManager(size_t sizeMB = 10);
    ~MemoryManager();

public:
    static MemoryManager& getInstance(size_t sizeMB = 10);

    template <typename T>
    int allocate(T value) {
        if (sizeof(T) > remainingMemory()) {
            throw std::runtime_error("Not enough memory");
        }
        int addr = nextAddress++;
        T* obj = new T(value);
        memory[addr] = {obj, sizeof(T)};
        return addr;
    }

    template <typename T>
    T& get(int address) {
        auto it = memory.find(address);
        if (it == memory.end()) {
            throw std::runtime_error("Invalid address");
        }
        return *static_cast<T*>(it->second.first);
    }

    void deallocate(int address);

    size_t remainingMemory() const {
        return memorySize - (memory.size() * sizeof(void*));
    }

    std::string generateDump() const;
};

#endif // MEMORYMANAGER_H