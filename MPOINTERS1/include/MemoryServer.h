//
// Created by lasle on 3/30/2025.
//

#ifndef MEMORYSERVER_H
#define MEMORYSERVER_H

#include "MemoryManager.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

class MemoryServer {
private:
    SOCKET serverSocket;
    const int PORT = 8080;

    void handleClient(SOCKET clientSocket);

public:
    MemoryServer();
    ~MemoryServer();
    void start();
};

#endif // MEMORYSERVER_H