//
// Created by Lenovo on 22/3/2025.
//

#ifndef MPOINTER_H
#define MPOINTER_H

#include <iostream>
#include <unordered_map>

class MemoryManager;

template <typename T>
class MPointer {
private:
    int address; // Simulated address
    static MemoryManager* manager; // Pointer to the memory manager

public:
    MPointer() : address(-1) {}

    static MPointer<T> New();

    T& operator*();
    T* operator->();
    MPointer<T>& operator=(const MPointer<T>& other);

    bool operator==(const MPointer<T>& other) const {
        return this->address == other.address;
    }

    bool isValid() const { return address != -1; }

    int getAddress() const { return address; }
};

// Define MemoryManager separately
#endif // MPOINTER_H

