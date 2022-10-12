cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME "RayTracing")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

file(GLOB TARGET_CPP_FILES "*.cpp")
add_executable(${PROJECT_NAME} ${TARGET_CPP_FILES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined)
# target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=undefined)