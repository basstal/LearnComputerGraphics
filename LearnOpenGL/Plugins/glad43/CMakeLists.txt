project(glad43)

if(NOT TARGET glad43)
    add_library(glad43 glad.c)
    include_directories(..)
endif()