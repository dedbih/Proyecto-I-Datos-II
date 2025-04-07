#include "MemoryManager.h"
#include "MPointer.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class MPointer {
private:
    int id;

public:
    MPointer(int id) : id(id) {}
    void set(int value);
    int get();
    std::string sendCommand(const std::string &cmd);
};

bool operator==(std::nullptr_t) const {
        return address == -1;
    }

void MPointer::set(int value) {
    sendCommand("SET " + std::to_string(id) + " " + std::to_string(value));
}

int MPointer::get() {
    std::string response = sendCommand("GET " + std::to_string(id));
    return std::stoi(response);
}

std::string MPointer::sendCommand(const std::string &cmd) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock" << std::endl;
        return "";
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error al crear socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return "";
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Error en connect: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return "";
    }

    send(sock, cmd.c_str(), cmd.length(), 0);
    recv(sock, buffer, sizeof(buffer), 0);

    closesocket(sock);
    WSACleanup();
    return std::string(buffer);
}
