cmake_minimum_required(VERSION 4.3)
project(dev_challenge)

set(CMAKE_CXX_STANDARD 20)

add_executable(dev_challenge src/main.cpp
        include/antworld.h
        src/antworld.cpp
        include/utility_functions.h
        src/useable_functions.cpp
        src/applicant_solution.cpp)
