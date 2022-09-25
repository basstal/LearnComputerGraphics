cmake_minimum_required(VERSION 3.10)

set(ROOT_PATH 
    "${CMAKE_CURRENT_SOURCE_DIR}/../..")
get_filename_component(ROOT_PATH ${ROOT_PATH} REALPATH)
set(PLUGINS "${ROOT_PATH}/Plugins")
set(SCRIPTS "${ROOT_PATH}/Scripts")
file(GLOB ROOT_SOURCE *.cpp)
subdirs(${PLUGINS}/glad43 ${PLUGINS}/imgui ${SCRIPTS} ${PLUGINS}/others)

set(PROJECT_NAME "Debugging")

set(CMAKE_CXX_STANDARD 11)

# Project name
project(${PROJECT_NAME})

# Project source
add_executable(${PROJECT_NAME} ${ROOT_SOURCE})

target_link_libraries(${PROJECT_NAME}
    "${PLUGINS}/glfw-3.3.4/lib-vc2019/glfw3_mt.lib"
    "${PLUGINS}/assimp-5.0.1/lib/assimp-vc142-mtd.lib"
    "${PLUGINS}/assimp-5.0.1/lib/zlibstaticd.lib"
    "${PLUGINS}/assimp-5.0.1/lib/IrrXMLd.lib"
    glad43
    lcosphere
    imgui
    common)

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${PLUGINS}/glfw-3.3.4/include"
    "${PLUGINS}/imgui"
    "${PLUGINS}/assimp-5.0.1/include"
    "${PLUGINS}"
    "${SCRIPTS}/include")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                    # Adds a post-build event to project
    COMMAND ${CMAKE_COMMAND} -E copy_if_different                       # which executes "cmake - E copy_if_different..."
    "${PLUGINS}/assimp-5.0.1/lib/assimp-vc142-mtd.dll"        # <--this is in-file
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)                                 # <--this is out-file path
    
if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /wd4267") # Suppress 'warning C4267
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /wd4244") # Suppress 'warning C4244: 'conversion' conversion from 'type1' to 'type2', possible loss of data'
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /wd4305") # Suppress 'warning C4305: 'argument': truncation from 'double' to 'float''
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT.LIB")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
endif()