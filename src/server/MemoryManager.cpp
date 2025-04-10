#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#include <string>
#include <vector>


#pragma comment(lib, "ws2_32.lib")

struct MemoryBlock {
    uint32_t id{};
    void* start{};
    size_t size{};
    std::string type;
    int ref_count{};
    bool is_free{};
};

class MemoryManager {
private:
    struct FreeBlock {
        void* address;
        size_t size;
    };

    std::unordered_map<uint32_t, MemoryBlock> blocks_;
    std::vector<FreeBlock> free_blocks_;
    void* memory_pool_;
    size_t pool_size_;
    uint32_t next_id_ = 1;
    std::mutex mutex_;
    std::string dump_folder_;
    bool running_ = true;
    std::thread gc_thread_;
    SOCKET server_socket_;
    WSADATA wsaData_{};

    void initializeFreeBlocks() {
        FreeBlock initial_block{};
        initial_block.address = memory_pool_;
        initial_block.size = pool_size_;
        free_blocks_.push_back(initial_block);
    }

    void* allocateMemory(size_t size) {
        for (auto it = free_blocks_.begin(); it != free_blocks_.end(); ++it) {
            if (it->size >= size) {
                void* alloc_address = it->address;
                size_t remaining = it->size - size;

                if (remaining > 0) {
                    FreeBlock new_free = {static_cast<char*>(it->address) + size, remaining};
                    free_blocks_.insert(it, new_free);
                }

                free_blocks_.erase(it);
                return alloc_address;
            }
        }
        return nullptr;
    }

    void freeMemory(void* address, size_t size) {
        FreeBlock new_block = {address, size};
        auto it = free_blocks_.begin();

        while (it != free_blocks_.end() && it->address < address) ++it;

        if (it != free_blocks_.begin()) {
            auto prev = std::prev(it);
            if (static_cast<char*>(prev->address) + prev->size == address) {
                prev->size += size;
                address = prev->address;
                size = prev->size;
                free_blocks_.erase(prev);
                it = free_blocks_.begin();
            }
        }

        if (it != free_blocks_.end() && static_cast<char*>(address) + size == it->address) {
            it->address = address;
            it->size += size;
        } else {
            free_blocks_.insert(it, new_block);
        }
    }

    std::string getDumpFilename() {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        auto timer = std::chrono::system_clock::to_time_t(now);
        std::tm bt{};
        localtime_s(&bt, &timer);
        std::ostringstream oss;
        oss << dump_folder_ << "\\dump_"
            << std::put_time(&bt, "%Y%m%d_%H%M%S_") << ms.count() << ".txt";
        return oss.str();
    }

    void dumpMemoryState() {
        std::string filename = getDumpFilename();
        std::ofstream dump_file(filename);
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : blocks_) {
            const MemoryBlock& block = pair.second;
            dump_file << "[ID: " << block.id << ", Tipo: " << block.type
                      << ", Tamaño: " << block.size << ", RefCount: " << block.ref_count
                      << ", Libre: " << (block.is_free ? "Sí" : "No") << "]\n";
        }
        dump_file.close();
    }

    void garbageCollector() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto it = blocks_.begin(); it != blocks_.end(); ) {
                if (it->second.ref_count == 0 && !it->second.is_free) {
                    freeMemory(it->second.start, it->second.size);
                    it = blocks_.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    std::string processCommand(const std::string& cmd) {
        std::istringstream iss(cmd);
        std::string action;
        iss >> action;

        if (action == "CREATE") {
            std::string type;
            size_t size;
            iss >> type >> size;

            std::lock_guard<std::mutex> lock(mutex_);
            void* allocated = allocateMemory(size);
            if (!allocated) return "ERROR No hay suficiente memoria";

            MemoryBlock block;
            block.id = next_id_++;
            block.size = size;
            block.type = type;
            block.ref_count = 1;
            block.is_free = false;
            block.start = allocated;

            blocks_[block.id] = block;
            dumpMemoryState();
            return "OK " + std::to_string(block.id);
        }
        else if (action == "DELETE") {
            uint32_t id;
            iss >> id;

            std::lock_guard<std::mutex> lock(mutex_);
            auto it = blocks_.find(id);
            if (it == blocks_.end()) return "ERROR ID no existe";

            it->second.ref_count--;
            return "OK";
        }
        else if (action == "INCREF") {
            uint32_t id;
            iss >> id;
            auto it = blocks_.find(id);
            if (it == blocks_.end()) return "ERROR ID no existe";
            it->second.ref_count++;
            return "OK";
        }
        else if (action == "DECREF") {
            uint32_t id;
            iss >> id;
            auto it = blocks_.find(id);
            if (it == blocks_.end()) return "ERROR ID no existe";
            it->second.ref_count--;
            return "OK";
        }
        return "ERROR Comando desconocido";
    }

    void handleClient(SOCKET client_socket) {
        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) break;

            std::string cmd(buffer, bytes_received);
            std::string response = processCommand(cmd);
            send(client_socket, response.c_str(), response.size(), 0);
        }
        closesocket(client_socket);
    }

public:
    MemoryManager(size_t pool_size, const std::string& dump_folder, uint16_t port)
        : pool_size_(pool_size), dump_folder_(dump_folder) {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }

        memory_pool_ = malloc(pool_size);
        if (!memory_pool_) throw std::runtime_error("Error al reservar memoria");
        initializeFreeBlocks();

        server_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket_ == INVALID_SOCKET) {
            throw std::runtime_error("Error al crear socket");
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(server_socket_, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
            throw std::runtime_error("Error en bind");
        }

        if (listen(server_socket_, SOMAXCONN) == SOCKET_ERROR) {
            throw std::runtime_error("Error en listen");
        }

        gc_thread_ = std::thread(&MemoryManager::garbageCollector, this);
    }

    ~MemoryManager() {
        running_ = false;
        if (gc_thread_.joinable()) gc_thread_.join();
        closesocket(server_socket_);
        free(memory_pool_);
        WSACleanup();
    }

    void run() {
        std::cout << "Servidor escuchando en puerto..." << std::endl;
        while (running_) {
            sockaddr_in client_addr{};
            int client_len = sizeof(client_addr);
            SOCKET client_socket = accept(server_socket_, (sockaddr*)&client_addr, &client_len);
            if (client_socket == INVALID_SOCKET) continue;
            std::thread(&MemoryManager::handleClient, this, client_socket).detach();
        }
    }
};

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <PUERTO> <TAMAÑO_MB> <CARPETA_DUMP>\n";
        return 1;
    }

    uint16_t port = std::stoi(argv[1]);
    size_t memsize = std::stoul(argv[2]) * 1024 * 1024;
    std::string dump_folder = argv[3];

    try {
        MemoryManager manager(memsize, dump_folder, port);
        manager.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}