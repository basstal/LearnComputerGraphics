cmake_minimum_required(VERSION 3.10)
project(RayTracing)

set(CMAKE_CXX_STANDARD 17)

file(GLOB CPP_SOURCE "*.cpp")
add_executable(RayTracing ${CPP_SOURCE})

if(MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /openmp")
endif()