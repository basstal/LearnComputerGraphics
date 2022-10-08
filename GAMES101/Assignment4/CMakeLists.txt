cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "BezierCurve")
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)
set(OpenCV_DIR "../Plugins/opencv-4.6.0/install")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})


if(MSVC)
    file(GLOB COPY_OPENCV_DLLS "${_OpenCV_LIB_PATH}/*.dll")
    foreach(DLL_TO_COPY IN LISTS COPY_OPENCV_DLLS)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                    # Adds a post-build event to project
            COMMAND ${CMAKE_COMMAND} -E copy_if_different                       # which executes "cmake - E copy_if_different..."
            ${DLL_TO_COPY}                                                      # <--this is in-file
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)                                 # <--this is out-file path
    endforeach()

    set(CMAKE_CXX_FLAGS "/MTd")
endif()