#include "MemoryManager.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <fstream>

MemoryManager::MemoryManager(size_t sizeMB) {
    memorySize = sizeMB * 1024 * 1024;  // Convertir MB a bytes
    memoryBlock = static_cast<char*>(malloc(memorySize));

    if (!memoryBlock) {
        throw std::runtime_error("Error al reservar memoria.");
    }
}

MemoryManager::~MemoryManager() {
    free(memoryBlock);
}

std::string MemoryManager::generateDump() {
    std::ostringstream dump;
    dump << "Estado de la memoria:\n";
    dump << "Tamaño: " << memorySize << " bytes\n";

    std::ofstream outFile("memory_dump.txt");
    outFile << dump.str();
    outFile.close();

    return dump.str();
}

void MemoryManager::handleClient(int clientSocket) {
    char buffer[1024] = {0};
    read(clientSocket, buffer, 1024);

    std::string command(buffer);
    if (command == "DUMP") {
        std::string dump = generateDump();
        send(clientSocket, dump.c_str(), dump.size(), 0);
    }
}
