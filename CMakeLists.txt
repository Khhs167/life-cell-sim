cmake_minimum_required(VERSION 3.23)
project(life)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(raylib)
add_subdirectory(inipp)

configure_file(simulation.ini simulation.ini COPYONLY)

add_executable(life main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib inipp)