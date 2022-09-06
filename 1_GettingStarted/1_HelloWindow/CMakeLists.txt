cmake_minimum_required(VERSION 3.10)

set(ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
get_filename_component(ROOT_PATH ${ROOT_PATH} REALPATH)
set(PLUGINS "${ROOT_PATH}/Plugins")
file(GLOB ROOT_SOURCE *.cpp)

set(PROJECT_NAME "HelloWindow")

# Project name
project(${PROJECT_NAME})

# Project source
add_executable(${PROJECT_NAME} ${ROOT_SOURCE})

subdirs(${PLUGINS}/glad)

target_link_libraries(${PROJECT_NAME} glad)

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${PLUGINS}/glfw-3.3.4/include"
    "${PLUGINS}")

if(MSVC)
    target_link_libraries(${PROJECT_NAME}
        "${PLUGINS}/glfw-3.3.4/lib-vc2019/glfw3_mt.lib")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT.LIB")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
endif()