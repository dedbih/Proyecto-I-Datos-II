#include "MemoryManager.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

class MemoryManager {
private:
    size_t memorySize;
    char* memoryBlock;

public:
    MemoryManager(size_t sizeMB);
    ~MemoryManager();
    std::string generateDump();
    void handleClient(SOCKET clientSocket);
};

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

void MemoryManager::handleClient(SOCKET clientSocket) {
    char buffer[1024] = {0};
    recv(clientSocket, buffer, 1024, 0);

    std::string command(buffer);
    if (command == "DUMP") {
        std::string dump = generateDump();
        send(clientSocket, dump.c_str(), dump.size(), 0);
    }
}
