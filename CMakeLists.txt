cmake_minimum_required(VERSION 3.16)

project(StrukdatPenjadwalanRuangan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(main_map main.cpp)
target_include_directories(main_map PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main_vector main2.cpp)
target_include_directories(main_vector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(run-map
    COMMAND main_map
    DEPENDS main_map
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    USES_TERMINAL
)

add_custom_target(run-main
    COMMAND main_map
    DEPENDS main_map
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    USES_TERMINAL
)

add_custom_target(run-vector
    COMMAND main_vector
    DEPENDS main_vector
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    USES_TERMINAL
)

add_custom_target(run-main2
    COMMAND main_vector
    DEPENDS main_vector
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    USES_TERMINAL
)
