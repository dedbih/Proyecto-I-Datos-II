cmake_minimum_required(VERSION 3.10)
project(MPOINTERS1)

set(CMAKE_CXX_STANDARD 17)

# Set include directory for ALL targets
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

# Windows-specific configurations
if(WIN32)
    target_link_libraries(ServerExe ws2_32)
    target_link_libraries(ClientExe ws2_32)
    target_link_libraries(TestExe ws2_32)
endif()