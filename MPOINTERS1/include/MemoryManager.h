//
// Created by Lenovo on 22/3/2025.
//

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <iostream>
#include <unordered_map>

class MemoryManager {
private:
    std::unordered_map<int, void*> memory; // Simulated memory storage
    int nextAddress = 0;

public:
    static MemoryManager& getInstance();

    template <typename T>
    int allocate(T value);

    template <typename T>
    T& get(int address);

    void deallocate(int address);
};

#endif // MEMORYMANAGER_H

