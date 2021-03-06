cmake_minimum_required(VERSION 2.6)
project(thechess)

enable_testing()

set(VERSION_MAJOR 3)
set(VERSION_MINOR 0)
set(VERSION_PATCH 9)

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(ADMIN_EMAIL thechess2008@gmail.com CACHE STRING "email of admin")

include(${CMAKE_CURRENT_SOURCE_DIR}/ubuntu-gcc.cmake)

file(GLOB thechess_sources
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*/*/*.cpp"
)

file(GLOB thechess_headers
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*/*/*.hpp"
)

subdirs(src)
subdirs(locales)
subdirs(files)

option(BUILD_DOC "Build documentation" ON)
if(BUILD_DOC)
    subdirs(doc)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/thechess-run
    ${CMAKE_CURRENT_BINARY_DIR}/thechess-run)

