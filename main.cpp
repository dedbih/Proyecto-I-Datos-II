#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <fstream>
#include <chrono>
#include <ctime>
#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using memorymanager::MemoryService;
using memorymanager::CreateRequest;
using memorymanager::CreateResponse;
using memorymanager::SetRequest;
using memorymanager::SetResponse;
using memorymanager::GetRequest;
using memorymanager::GetResponse;
using memorymanager::RefCountRequest;
using memorymanager::RefCountResponse;

class MemoryManagerService final : public MemoryService::Service {
private:
    size_t total_memory;
    std::vector<char> memory_block;
    std::unordered_map<int, std::pair<size_t, size_t>> allocations; // id -> (offset, size)
    std::unordered_map<int, int> ref_count;
    std::mutex mem_mutex;
    int next_id = 1;
    std::string dump_folder;

public:
    MemoryManagerService(size_t mem_size, std::string dump_path)
        : total_memory(mem_size * 1024 * 1024), memory_block(total_memory, 0), dump_folder(std::move(dump_path)) {}

    Status Create(ServerContext* context, const CreateRequest* request, CreateResponse* response) override {
        std::lock_guard<std::mutex> lock(mem_mutex);
        size_t size = request->size();
        if (size > total_memory) return Status::CANCELLED;

        int id = next_id++;
        allocations[id] = {0, size};
        ref_count[id] = 1;
        response->set_id(id);
        DumpMemory();
        return Status::OK;
    }

    Status Set(ServerContext* context, const SetRequest* request, SetResponse* response) override {
        std::lock_guard<std::mutex> lock(mem_mutex);
        int id = request->id();
        if (allocations.find(id) == allocations.end()) return Status::CANCELLED;

        memcpy(&memory_block[allocations[id].first], request->value().c_str(), allocations[id].second);
        DumpMemory();
        return Status::OK;
    }

    Status Get(ServerContext* context, const GetRequest* request, GetResponse* response) override {
        std::lock_guard<std::mutex> lock(mem_mutex);
        int id = request->id();
        if (allocations.find(id) == allocations.end()) return Status::CANCELLED;
        response->set_value(std::string(&memory_block[allocations[id].first], allocations[id].second));
        return Status::OK;
    }

    Status IncreaseRefCount(ServerContext* context, const RefCountRequest* request, RefCountResponse* response) override {
        std::lock_guard<std::mutex> lock(mem_mutex);
        int id = request->id();
        if (ref_count.find(id) == ref_count.end()) return Status::CANCELLED;
        ref_count[id]++;
        return Status::OK;
    }

    Status DecreaseRefCount(ServerContext* context, const RefCountRequest* request, RefCountResponse* response) override {
        std::lock_guard<std::mutex> lock(mem_mutex);
        int id = request->id();
        if (ref_count.find(id) == ref_count.end()) return Status::CANCELLED;

        ref_count[id]--;
        if (ref_count[id] == 0) {
            allocations.erase(id);
            ref_count.erase(id);
        }
        DumpMemory();
        return Status::OK;
    }

    void DumpMemory() {
        std::ofstream dump_file(dump_folder + "/dump_" + std::to_string(time(0)) + ".txt");
        for (const auto& pair : allocations) {
            dump_file << "ID: " << pair.first << " Offset: " << pair.second.first << " Size: " << pair.second.second << "\n";
        }
        dump_file.close();
    }
};

void RunServer(int port, size_t memsize, const std::string& dump_folder) {
    std::string server_address = "0.0.0.0:" + std::to_string(port);
    MemoryManagerService service(memsize, dump_folder);
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char* argv[]) {
    if (argc != 7 || std::string(argv[1]) != "--port" || std::string(argv[3]) != "--memsize" || std::string(argv[5]) != "--dumpFolder") {
        std::cerr << "Usage: ./mem-mgr --port LISTEN_PORT --memsize SIZE_MB --dumpFolder DUMP_FOLDER" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[2]);
    size_t memsize = std::stoull(argv[4]);
    std::string dump_folder = argv[6];

    RunServer(port, memsize, dump_folder);
    return 0;
}