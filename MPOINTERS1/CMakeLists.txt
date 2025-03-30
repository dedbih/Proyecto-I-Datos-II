cmake_minimum_required(VERSION 3.10)
project(MPOINTERS1)

set(CMAKE_CXX_STANDARD 17)

# Server executable
add_executable(ServerExe
        src/server/main.cpp
        src/server/MemoryManager.cpp
        src/server/MemoryServer.cpp
)

# Client executable
add_executable(ClientExe
        src/client/main.cpp
)

# Link Winsock for Windows
if(WIN32)
    target_link_libraries(ServerExe ws2_32)
endif()

# Include directories
target_include_directories(ServerExe PRIVATE include)
target_include_directories(ClientExe PRIVATE include)