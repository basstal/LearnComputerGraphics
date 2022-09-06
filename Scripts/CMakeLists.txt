project(LearnOpenGL_Common)

if(NOT TARGET common)
    file(GLOB ROOT_SOURCE *.cpp)
    add_library(common ${ROOT_SOURCE})

    include_directories(include ../Plugins ../Plugins/assimp-5.0.1/include)

    if(MSVC)
        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} /wd4244") # Suppress 'warning C4244: 'conversion' conversion from 'type1' to 'type2', possible loss of data'
    endif()
endif()