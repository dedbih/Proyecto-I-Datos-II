#ifndef MPOINTER_H
#define MPOINTER_H

#include <iostream>
#include <string>
#include <cstdint> // ✅ Cabecera necesaria
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

template <typename T>
class MPointer {
private:
    int id_;
    static SOCKET socket_;
    bool is_owner_;

    std::string sendCommand(const std::string& cmd) {
        send(socket_, cmd.c_str(), cmd.size(), 0);
        char buffer[1024];
        int bytes_received = recv(socket_, buffer, sizeof(buffer), 0);
        return std::string(buffer, bytes_received);
    }

public:
    MPointer() : id_(-1), is_owner_(false) {}
    MPointer(int id) : id_(id), is_owner_(false) {}

    static void Init(const std::string& host, uint16_t port) {
        WSADATA wsa_data;
        WSAStartup(MAKEWORD(2, 2), &wsa_data);
        socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
        connect(socket_, (sockaddr*)&server_addr, sizeof(server_addr));
    }

    static MPointer<T> New() {
        MPointer<T> temp; // ✅ Instancia temporal
        std::string response = temp.sendCommand("CREATE " + std::string(typeid(T).name()) + " " + std::to_string(sizeof(T)));
        if (response.substr(0, 2) == "OK") {
            int id = std::stoi(response.substr(3));
            return MPointer<T>(id);
        }
        throw std::runtime_error("Error al crear MPointer");
    }

    T& operator*() { // ✅ Retorna referencia
        std::string response = sendCommand("GET " + std::to_string(id_));
        if (response.substr(0, 2) == "OK") {
            T* value = reinterpret_cast<T*>(&response[3]);
            return *value;
        }
        throw std::runtime_error("Error al obtener valor");
    }

    // Sobrecarga del operador =
    MPointer<T>& operator=(const T& value) {
        std::string cmd = "SET " + std::to_string(id_) + " ";
        cmd.append(reinterpret_cast<const char*>(&value), sizeof(T)); // Binary data
        sendCommand(cmd);
        return *this;
    }

    // Sobrecarga del operador = para copiar MPointers
    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) {
            if (id_ != -1) {
                sendCommand("DECREF " + std::to_string(id_));
            }
            id_ = other.id_;
            sendCommand("INCREF " + std::to_string(id_)); // Add reference
            is_owner_ = true; // Transfer ownership
        }
        return *this;
    }

    // Destructor
    ~MPointer() {
        if (is_owner_) {
            sendCommand("DECREF " + std::to_string(id_));
        }
    }
};

template <typename T>
SOCKET MPointer<T>::socket_ = INVALID_SOCKET;

// Explicit template instantiation
template class MPointer<int>; // Add other types as needed
#endif //MPOINTER_H
