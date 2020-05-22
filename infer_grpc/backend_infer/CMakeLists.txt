cmake_minimum_required(VERSION 3.8)

project(backend_infer CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

set(CMAKE_CXX_STANDARD 11)

# link_directories(../tf_c_api-1.15.0/lib)
# include_directories(../tf_c_api-1.15.0/include)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

add_executable(dummy_backend dummy_backend.cpp  backend_infer.pb.cc backend_infer.grpc.pb.cc)

target_link_libraries(dummy_backend protobuf grpc grpc++)
