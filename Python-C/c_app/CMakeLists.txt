cmake_minimum_required(VERSION 3.16)
project(c_app)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

add_executable(c_app main.cpp)

add_executable(receive receive.c)

add_executable(send_a send_a.c)

add_executable(send_b send_b.c)

add_executable(shm_read shm_read.c)

add_executable(shm_write shm_write.c)


