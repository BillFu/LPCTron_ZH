cmake_minimum_required(VERSION 3.9)
project(tf_cpp_api_demo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MACOSX_RPATH 0)

include_directories(
        /usr/local/include/
        /usr/local/include/tf
        /usr/local/include/eigen3
        ~/abseil-cpp)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

link_directories(/usr/local/lib)
add_executable(tf_cpp_api_demo tf_cpp_api_demo.cpp )
target_link_libraries(tf_cpp_api_demo tensorflow_cc)