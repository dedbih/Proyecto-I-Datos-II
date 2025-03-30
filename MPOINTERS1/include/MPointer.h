//
// Created by Lenovo on 22/3/2025.
//

#ifndef MPOINTER_H
#define MPOINTER_H

// Forward declaration
class MemoryManager;

template <typename T>
class MPointer {
private:
    int address;
    static MemoryManager* manager;

public:
    MPointer() : address(-1) {}
    explicit MPointer(int addr) : address(addr) {}

    ~MPointer();

    static MPointer<T> New(T value = T());

    T& operator*();
    T* operator->();

    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) {
            address = other.address;
        }
        return *this;
    }

    bool operator==(const MPointer<T>& other) const {
        return address == other.address;
    }

    bool isValid() const { return address != -1; }
    int getAddress() const { return address; }
};

// Include the actual MemoryManager definition after template declaration
#include "MemoryManager.h"

// Destructor implementation
template <typename T>
MPointer<T>::~MPointer() {
    if (address != -1) {
        MemoryManager::getInstance().deallocate(address);
    }
}

template <typename T>
MemoryManager* MPointer<T>::manager = &MemoryManager::getInstance();

template <typename T>
MPointer<T> MPointer<T>::New(T value) {
    return MPointer<T>(MemoryManager::getInstance().allocate<T>(value));
}

template <typename T>
T& MPointer<T>::operator*() {
    return MemoryManager::getInstance().get<T>(address);
}

template <typename T>
T* MPointer<T>::operator->() {
    return &MemoryManager::getInstance().get<T>(address);
}

#endif // MPOINTER_H