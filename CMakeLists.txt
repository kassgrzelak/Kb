cmake_minimum_required(VERSION 3.21)
project(kflat)

set(CMAKE_CXX_STANDARD 20)

add_library(kflat_lib
        Chunk.cpp
        Chunk.hpp
        debug.cpp
        debug.hpp
        Value.cpp
        Value.hpp
)

target_include_directories(kflat_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(kflat
        main.cpp
)

target_link_libraries(kflat PRIVATE kflat_lib)

add_subdirectory(kflat-tests)
