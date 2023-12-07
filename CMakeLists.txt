cmake_minimum_required(VERSION 3.23)
project(frequent_directions)

set(CMAKE_CXX_STANDARD 14)

add_executable(frequent_directions main.cpp opencsv.cpp opencsv.h frequentdirections.h)

