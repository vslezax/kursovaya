cmake_minimum_required(VERSION 3.16)
project(kursovaya)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
set(BUILD_SHARED_LIBS OFF)

add_executable(kursovaya main.cpp tree.h tree.cpp Fano.h Fano.cpp executeFano.cpp executeFano.h)