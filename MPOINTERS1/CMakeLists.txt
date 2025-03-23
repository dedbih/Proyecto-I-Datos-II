cmake_minimum_required(VERSION 3.10)
project(MPOINTERS1)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Add the executable with correct file paths
add_executable(MPOINTERS1
        src/client/main.cpp
        src/client/MPointer.cpp
        src/server/main.cpp
        src/server/MemoryManager.cpp
        src/test/ListTest.cpp
        include/MPointer.h
        include/MemoryManager.h
)

