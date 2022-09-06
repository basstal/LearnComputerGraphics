project(imgui)

if(NOT TARGET imgui)
    file(GLOB ROOT_SOURCE *.cpp)
    add_library(imgui ${ROOT_SOURCE})
    include_directories(../glfw-3.3.4/include ..)
endif()