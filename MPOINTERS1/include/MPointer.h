//
// Created by Lenovo on 22/3/2025.
//

#ifndef MPOINTER_H
#define MPOINTER_H

#include "MemoryManager.h"

template <typename T>
class MPointer {
private:
    int address;

public:
    MPointer() : address(-1) {}
    explicit MPointer(int addr) : address(addr) {}

    ~MPointer() {
        if (address != -1) {
            MemoryManager::getInstance().deallocate(address);
        }
    }

    static MPointer<T> New(T value = T()) {
        return MPointer<T>(MemoryManager::getInstance().allocate<T>(value));
    }

    T& operator*() {
        return MemoryManager::getInstance().get<T>(address);
    }

    T* operator->() {
        return &MemoryManager::getInstance().get<T>(address);
    }

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

#endif // MPOINTER_H