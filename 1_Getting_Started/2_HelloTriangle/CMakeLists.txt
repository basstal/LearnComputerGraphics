cmake_minimum_required(VERSION 3.10)

set(ROOT_PATH 
    "${CMAKE_CURRENT_SOURCE_DIR}/../..")
get_filename_component(ROOT_PATH ${ROOT_PATH} REALPATH)
# message("${ROOT_PATH}")

set(PROJECT_NAME "HelloTriangle")

# Project name
project(${PROJECT_NAME})

# lib source
add_library(glad
    "${ROOT_PATH}/Plugins/glad/glad.c")

target_include_directories(glad PUBLIC
    "${ROOT_PATH}/Plugins")

# project source
add_executable(${PROJECT_NAME} 
    HelloTriangle.cpp)

target_link_libraries(${PROJECT_NAME}
    "${ROOT_PATH}/Plugins/glfw-3.3.4/lib-vc2019/glfw3.lib"
    glad)

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${ROOT_PATH}/Plugins/glfw-3.3.4/include"
    "${ROOT_PATH}/Plugins")

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB")
endif()