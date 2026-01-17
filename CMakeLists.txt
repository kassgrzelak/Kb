cmake_minimum_required(VERSION 3.21)
project(kflat)

set(CMAKE_CXX_STANDARD 20)

add_library(kflat_lib
        src/Chunk.cpp
        include/Chunk.hpp
        src/debug.cpp
        include/debug.hpp
        src/Value.cpp
        include/Value.hpp
        src/VM.cpp
        include/VM.hpp
        src/compiler.cpp
        include/compiler.hpp
        src/Scanner.cpp
        include/Scanner.hpp
)

target_include_directories(kflat_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(kflat
        src/main.cpp
)

target_link_libraries(kflat PRIVATE kflat_lib)

add_subdirectory(kflat-tests)
