//
// Created by Lenovo on 22/3/2025.
//

#ifndef MPOINTER_H
#define MPOINTER_H

#include "MemoryManager.h"
#include <string>

template <typename T>
class MPointer {
private:
    int address;
    static MemoryManager* manager;

public:
    MPointer() : address(-1) {}
    explicit MPointer(int addr) : address(addr) {}

    static MPointer<T> New(T value = T()) {
        int addr = manager->getInstance().allocate<T>(value);
        return MPointer<T>(addr);
    }

    T& operator*() {
        return manager->getInstance().get<T>(address);
    }

    T* operator->() {
        return &manager->getInstance().get<T>(address);
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

    // Network communication
    static std::string sendCommand(const std::string& cmd);
};

template <typename T>
MemoryManager* MPointer<T>::manager = &MemoryManager::getInstance();

#endif // MPOINTER_H