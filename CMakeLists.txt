cmake_minimum_required(VERSION 4.3)
project(dev_challenge)

set(CMAKE_CXX_STANDARD 20)

add_executable(dev_challenge main.cpp
        antworld.h
        antworld.cpp
        utility_functions.h
        useable_functions.cpp
        applicant_solution.cpp)
