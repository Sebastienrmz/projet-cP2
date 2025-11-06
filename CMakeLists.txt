cmake_minimum_required(VERSION 3.16)
project(TI301_Partie1 C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# === liste tous les .c qui doivent être compilés dans l'exécutable ===
add_executable(markov_graph
        src/main.c
        src/graph.c
        # si tu as d'autres .c à compiler dans le même exécutable, ajoute-les ici
        # src/testarthur.c
        # src/testraph.c
        # src/testseb.c
)

target_include_directories(markov_graph PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
