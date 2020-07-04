cmake_minimum_required(VERSION 3.16)
project(backend)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

add_executable(backend_server backend_server2.cpp)
set_target_properties(backend_server PROPERTIES CXX_STANDARD 14)

target_link_libraries(backend_server event event_core event_pthreads event_extra
        boost_thread boost_system boost_chrono)
