cmake_minimum_required(VERSION 3.8)

project(posix_ipc C)

set(CMAKE_C_STANDARD 99)

add_executable(logger logger.c)
add_executable(client client.c)
add_executable(posix_sem posix_sem.c)

add_executable(mq_server mq_server.c)
add_executable(mq_client mq_client.c)
