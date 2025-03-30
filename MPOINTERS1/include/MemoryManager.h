//
// Created by Lenovo on 22/3/2025.
//

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <unordered_map>
#include <stdexcept>
#include <string>

class MemoryManager {
private:
    std::unordered_map<int, void*> memory;
    int nextAddress = 0;
    size_t memorySize;
    char* memoryBlock;

    MemoryManager(size_t sizeMB = 10); // Default 10MB
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
        memory[addr] = static_cast<void*>(obj);
        return addr;
    }

    template <typename T>
    T& get(int address) {
        auto it = memory.find(address);
        if (it == memory.end()) {
            throw std::runtime_error("Invalid address");
        }
        return *static_cast<T*>(it->second);
    }

    void deallocate(int address);
    size_t remainingMemory() const;
    std::string generateDump() const;
};

#endif // MEMORYMANAGER_H

