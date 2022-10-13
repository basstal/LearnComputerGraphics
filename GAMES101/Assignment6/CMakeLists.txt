cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME "RayTracing")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

file(GLOB TARGET_CPP_FILES "*.cpp")
add_executable(${PROJECT_NAME} ${TARGET_CPP_FILES})
