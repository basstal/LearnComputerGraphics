cmake_minimum_required(VERSION 3.10)

set(ROOT_PATH 
    "${CMAKE_CURRENT_SOURCE_DIR}/../..")
get_filename_component(ROOT_PATH ${ROOT_PATH} REALPATH)
set(PLUGINS "${ROOT_PATH}/Plugins")
set(SCRIPTS "${ROOT_PATH}/Scripts")
file(GLOB ROOT_SOURCE *.cpp)
subdirs(${PLUGINS}/glad ${PLUGINS}/imgui ${SCRIPTS})

set(PROJECT_NAME "FaceCulling")

# Project name
project(${PROJECT_NAME})

# Project source
add_executable(${PROJECT_NAME} ${ROOT_SOURCE})

target_link_libraries(${PROJECT_NAME}
    "${PLUGINS}/glfw-3.3.4/lib-vc2019/glfw3_mt.lib"
    glad
    imgui
    common)

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${PLUGINS}/glfw-3.3.4/include"
    "${PLUGINS}/imgui"
    "${PLUGINS}"
    "${SCRIPTS}/include")

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /wd4244") # Suppress 'warning C4244: 'conversion' conversion from 'type1' to 'type2', possible loss of data'
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /wd4305") # Suppress 'warning C4305: 'argument': truncation from 'double' to 'float''
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT.LIB")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
endif()