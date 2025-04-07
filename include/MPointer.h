//
// Created by Lenovo on 22/3/2025.
//
#ifndef MPOINTER_H
#define MPOINTER_H

#include "MemoryManager.h"
#include <cstddef>  // For nullptr_t

template <typename T> // me permite usar cualquier tipo de datos
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

    template <typename... Args>
    static MPointer<T> New(Args&&... args) {
        return MPointer<T>(
            MemoryManager::getInstance().allocate<T>(
                T(std::forward<Args>(args)...)
            ));
    }

    
    bool operator==(std::nullptr_t) const {
        return address == -1;
    }

    bool operator!=(std::nullptr_t) const {
        return address != -1;
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