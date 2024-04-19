cmake_minimum_required(VERSION 3.24)
project(MineSweeper)

set(CMAKE_CXX_STANDARD 14)

add_executable(MineSweeper main.cpp
        cmake-build-debug/Tile.cpp
        cmake-build-debug/Board.cpp
        cmake-build-debug/Timer.cpp
        cmake-build-debug/Counter.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(c:/SFML/include/SFML)
target_link_libraries(MineSweeper sfml-system sfml-window sfml-graphics sfml-audio)