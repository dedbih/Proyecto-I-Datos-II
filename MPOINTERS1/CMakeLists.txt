cmake_minimum_required(VERSION 3.10)
project(MPointersProject)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the executable with correct file paths
add_executable(MPOINTERS1
        src/client/main.cpp         # Client main file
        src/client/MPointer.cpp     # Client MPointer implementation
        src/server/main.cpp         # Server main file
        src/server/MemoryManager.cpp # Server MemoryManager implementation
        src/test/ListTest.cpp           # Test files
)
