cmake_minimum_required(VERSION 3.23)
project(frequent_directions)

set(CMAKE_CXX_STANDARD 17)

find_package(blaze REQUIRED)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)
find_package(OpenBLAS REQUIRED)
find_package(Armadillo REQUIRED)



add_executable(frequent_directions main.cpp
        opencsv.cpp
        opencsv.h
        frequent_directions.cpp
        frequent_directions.h)
target_link_libraries(frequent_directions PRIVATE blaze::blaze)
target_link_libraries(frequent_directions PRIVATE LAPACK::LAPACK)
target_link_libraries(frequent_directions PRIVATE BLAS::BLAS)
target_link_libraries(frequent_directions PRIVATE OpenBLAS::OpenBLAS)
target_link_libraries(frequent_directions PRIVATE ${ARMADILLO_LIBRARIES})

