cmake_minimum_required(VERSION 3.9)
project(tf_cpp_api_demo)

set(CMAKE_CXX_STANDARD 14)

include_directories(
        /usr/local/include/tf
        /usr/local/include/eigen3
        ~/abseil-cpp)

link_directories(./libs/tensorflow/lib/macOS)
add_executable(tf_cpp_api_demo tf_cpp_api_demo.cpp )
target_link_libraries(tf_cpp_api_demo tensorflow_cc)