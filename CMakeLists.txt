cmake_minimum_required(VERSION 3.16)
project(projet_cP2 C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(markov_graph
        src/main.c
        src/graph.c
)

target_include_directories(markov_graph PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
