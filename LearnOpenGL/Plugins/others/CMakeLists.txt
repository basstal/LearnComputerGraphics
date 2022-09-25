project(Icosphere)

if(NOT TARGET lcosphere)
    add_library(lcosphere Icosphere.cpp)

    include_directories(..)

    if(MSVC)
        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} /wd4267")
        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} /wd4244")
    endif()
endif()