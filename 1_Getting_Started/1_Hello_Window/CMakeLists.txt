cmake_minimum_required(VERSION 3.10)

set(ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
get_filename_component(ROOT_PATH ${ROOT_PATH} REALPATH)
message("${ROOT_PATH}")

# Project name
project(HelloWorld)

# lib source
add_library(glad
"${ROOT_PATH}/Plugins/glad/glad.c"
)

target_include_directories(glad PUBLIC
"${ROOT_PATH}/Plugins"
)

# project source
add_executable(HelloWorld HelloWorld.cpp)

target_link_libraries(HelloWorld
"${ROOT_PATH}/Plugins/glfw-3.3.4/lib-vc2019/glfw3.lib"
glad
)

target_include_directories(HelloWorld PUBLIC 
"${ROOT_PATH}/Plugins/glfw-3.3.4/include"
"${ROOT_PATH}/Plugins"
)