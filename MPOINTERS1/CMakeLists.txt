cmake_minimum_required(VERSION 3.10)
project(MPOINTERS1)

set(CMAKE_CXX_STANDARD 17)

# Include directories for all targets
include_directories(include)

# Server executable
add_executable(ServerExe
        src/server/main.cpp
        src/server/MemoryManager.cpp
)

# Client executable
add_executable(ClientExe
        src/client/main.cpp
)

# Windows-specific configurations
if(WIN32)
    target_link_libraries(ServerExe ws2_32)
    target_link_libraries(ClientExe ws2_32)
endif()

# Additional include directories (redundant but safe)
target_include_directories(ServerExe PRIVATE include)
target_include_directories(ClientExe PRIVATE include)