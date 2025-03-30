//
// Created by lasle on 3/30/2025.
//
#include "MemoryManager.h"
#include "MemoryServer.h"
#include <iostream>

MemoryServer::MemoryServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        throw std::runtime_error("Bind failed");
    }
}

MemoryServer::~MemoryServer() {
    closesocket(serverSocket);
    WSACleanup();
}

void MemoryServer::start() {
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) continue;

        handleClient(clientSocket);
        closesocket(clientSocket);
    }
}

void MemoryServer::handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) return;

    std::string command(buffer, bytesReceived);
    std::string response;

    if (command == "DUMP") {
        response = MemoryManager::getInstance().generateDump();
    }
    // Add more command handling here

    send(clientSocket, response.c_str(), response.size(), 0);
}