cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME "Rasterizer")
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)

set(Eigen3_DIR "../Plugins/eigen-3.4.0/share/eigen3/cmake")
set(OpenCV_DIR "../Plugins/opencv-4.6.0/install")
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories("${EIGEN3_INCLUDE_DIR}" "${OpenCV_INCLUDE_DIRS}")

file(GLOB TARGET_CPP_FILES "*.cpp")
add_executable(${PROJECT_NAME} ${TARGET_CPP_FILES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})

file(GLOB COPY_OPENCV_DLLS "${_OpenCV_LIB_PATH}/*.dll")
foreach(DLL_TO_COPY IN LISTS COPY_OPENCV_DLLS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                    # Adds a post-build event to project
        COMMAND ${CMAKE_COMMAND} -E copy_if_different                       # which executes "cmake - E copy_if_different..."
        ${DLL_TO_COPY}                                                      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)                                 # <--this is out-file path
endforeach()

if(MSVC)
    set(CMAKE_CXX_FLAGS "/MTd")
endif()