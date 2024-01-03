cmake_minimum_required(VERSION 3.23)
project(frequent_directions)

find_package(Blaze REQUIRED)
message(STATUS "Blaze version: ${Blaze_VERSION}")

find_package(LAPACK REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(frequent_directions main.cpp
)

target_link_libraries(frequent_directions PRIVATE LAPACK::LAPACK)
