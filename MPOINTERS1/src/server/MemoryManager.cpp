#include "MemoryManager.h"
#include <fstream>
#include <sstream>

MemoryManager::MemoryManager(size_t sizeMB) {
    memorySize = sizeMB * 1024 * 1024;
    memoryBlock = static_cast<char*>(malloc(memorySize));
    if (!memoryBlock) throw std::runtime_error("Memory allocation failed");
}

MemoryManager::~MemoryManager() {
    for (auto& entry : memory) {
        delete static_cast<char*>(entry.second.first);
    }
    free(memoryBlock);
}

std::string MemoryManager::generateDump() const {
    std::ostringstream oss;
    oss << "Memory Status:\n";
    oss << "Total: " << memorySize << " bytes\n";
    oss << "Used: " << memory.size() * sizeof(void*) << " bytes\n";
    oss << "Available: " << remainingMemory() << " bytes\n";
    return oss.str();
}
