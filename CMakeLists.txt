cmake_minimum_required(VERSION 3.16)
project(kursovaya)

set(CMAKE_CXX_STANDARD 14)

add_executable(kursovaya main.cpp tree.h tree.cpp Fano.h Fano.cpp executeFano.cpp executeFano.h)