cmake_minimum_required(VERSION 3.30)
project(untitled)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories("${CMAKE_SOURCE_DIR}/include")

# Define the executable
add_executable(MemoryManager
        src/server/MemoryManager.cpp
)
add_executable(MPointerClient
        src/client/MPointer.cpp
)

# Link Windows socket library if on Windows
if(WIN32)
    target_link_libraries(MemoryManager ws2_32)
    target_link_libraries(MPointerClient ws2_32)
endif()


