cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)
enable_testing()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
set(ENV{LD_LIBRARY_PATH} ${CMAKE_BINARY_DIR}/lib)


include_directories(include)
add_subdirectory(src)
