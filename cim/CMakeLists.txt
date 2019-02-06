
cmake_minimum_required(VERSION 3.1)
project(cim C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/modules/libgit2/cmake/Modules/")
SET(gitlibrootdir ${CMAKE_CURRENT_SOURCE_DIR}/modules/libgit2)

set(OLD_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_SOURCE_DIR ${gitlibrootdir})
set(CMAKE_CURRENT_SOURCE_DIR ${gitlibrootdir})
set(PROJECT_SOURCE_DIR ${gitlibrootdir})

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

set(CMAKE_SOURCE_DIR ${OLD_DIR})
set(CMAKE_CURRENT_SOURCE_DIR ${OLD_DIR})
set(PROJECT_SOURCE_DIR ${OLD_DIR})

set(SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cim/cim.c"

        )

add_executable(cim ${SOURCE_FILES})