cmake_minimum_required(VERSION 3.10)
project(Rasterizer)
set(CMAKE_CXX_STANDARD 17)

set(Eigen3_DIR "../Plugins/eigen-3.4.0/share/eigen3/cmake")
set(OpenCV_DIR "../Plugins/opencv-4.6.0/")

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories("${EIGEN3_INCLUDE_DIR}")


add_executable(Rasterizer main.cpp rasterizer.hpp rasterizer.cpp Triangle.hpp Triangle.cpp)
message("OpenCV_LIBS : ${OpenCV_LIBS}")
target_link_libraries(Rasterizer "${OpenCV_LIBS}")
