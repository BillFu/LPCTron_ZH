cmake_minimum_required(VERSION 3.9)

project(backend_infer CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

message(STATUS "test message...")

if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif (CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif (CMAKE_HOST_UNIX)
    set(LINUX 1)
endif ()

if (LINUX)
    message(STATUS "using LINUX configure...")

    link_directories(../libtensorflow-cpu-linux-x86_64-1.15.0/lib)
    include_directories(../libtensorflow-cpu-linux-x86_64-1.15.0/include)

    link_directories(../libsamplerate/build)
    include_directories(../libsamplerate/src)
elseif (MACOS)
    message(STATUS "using MacOS configure...")

    link_directories(../tf_c_api-1.15.0/lib)
    include_directories(../tf_c_api-1.15.0/include)

    link_directories(~/libsamplerate/build)
    include_directories(~/libsamplerate/src)
endif()

add_executable(backend_server
        backend_server.cpp
        backend_implement2.cpp
        wav_utils_v2.cpp
        backend_infer.pb.cc
        backend_infer.grpc.pb.cc
        TFUtils.cpp
        nnet_data.cpp
        lpcnet.cpp nnet.cpp  freq.cpp
        kiss_fft.cpp pitch.cpp celt_lpc.cpp vec_avx.h)

target_compile_definitions(backend_server PRIVATE __AVX__=1 TACOTRON2=1)

target_compile_options(backend_server
        PRIVATE "-mavx2"
        PRIVATE "-mfma"
        PRIVATE "-mavx"
        PRIVATE "-mf16c")

target_compile_features(backend_server PUBLIC cxx_std_11)

# target_link_libraries(dummy_backend protobuf grpc grpc++)
target_link_libraries(backend_server samplerate protobuf grpc grpc++ tensorflow m)