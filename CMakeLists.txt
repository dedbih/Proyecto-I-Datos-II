cmake_minimum_required(VERSION 3.10)
project(MPOINTERS1)

set(CMAKE_CXX_STANDARD 17)

# Set ABSOLUTE include path for all targets
include_directories("${CMAKE_SOURCE_DIR}/include")

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

# Test executable
add_executable(TestExe
        src/test/ListTest.cpp
)
# Explicitly set include directories for each target
target_include_directories(ServerExe PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(ClientExe PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(TestExe PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Windows-specific configurations
if(WIN32)
    target_link_libraries(ServerExe ws2_32)
    target_link_libraries(ClientExe ws2_32)
    target_link_libraries(TestExe ws2_32)
endif()