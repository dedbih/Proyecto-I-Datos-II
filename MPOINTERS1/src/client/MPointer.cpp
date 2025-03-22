#include "MPointer.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

MPointer::MPointer(int id) : id(id) {}

void MPointer::set(int value) {
    sendCommand("SET " + std::to_string(id) + " " + std::to_string(value));
}

int MPointer::get() {
    std::string response = sendCommand("GET " + std::to_string(id));
    return std::stoi(response);
}

std::string MPointer::sendCommand(const std::string &cmd) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    send(sock, cmd.c_str(), cmd.length(), 0);

    char buffer[1024] = {0};
    read(sock, buffer, 1024);

    close(sock);
    return std::string(buffer);
}
