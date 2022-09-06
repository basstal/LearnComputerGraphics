project(glad)

if(NOT TARGET glad)
    add_library(glad glad.c)
    include_directories(..)
endif()