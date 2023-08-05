cmake_minimum_required(VERSION 3.0)
project(plugpp-example VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
# The '-fno-gnu-unique' flag is really important here for plugin reloading
set(COMMON_FLAGS "-m32 -mtune=native -Wall -fPIC -fvisibility=hidden -fno-gnu-unique")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O1 ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O1 ${COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g -m32")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external/cod4-plugpp)

add_library(plugpp-example SHARED
    src/main.cpp
)
target_link_libraries(plugpp-example
    PRIVATE cod4-plugpp
)
target_include_directories(plugpp-example
    PRIVATE include
)

set_property(TARGET plugpp-example PROPERTY CXX_STANDARD 17)
set_property(TARGET plugpp-example PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET plugpp-example PROPERTY CXX_EXTENSIONS OFF)
