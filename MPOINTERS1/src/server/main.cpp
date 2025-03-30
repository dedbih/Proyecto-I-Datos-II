#include "MemoryServer.h"
#include <iostream>

int main() {
    try {
        MemoryServer server;
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}