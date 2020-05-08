cmake_minimum_required(VERSION 3.8)

project(posix_ipc C)

set(CMAKE_C_STANDARD 99)

add_executable(logger logger.c)
add_executable(client client.c)
add_executable(posix_sem posix_sem.c)
