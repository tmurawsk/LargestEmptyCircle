cmake_minimum_required(VERSION 3.6)
project(LEC)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Point.h Voronoi.cpp Voronoi.h Gui.cpp Gui.h Edge.h Event.h Parabola.h)
add_executable(LEC ${SOURCE_FILES})