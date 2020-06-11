cmake_minimum_required(VERSION 3.9)

project(backend_infer CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)


link_directories(../tf_c_api-1.15.0/lib)
include_directories(../tf_c_api-1.15.0/include)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

link_directories(~/libsamplerate/build)
include_directories(~/libsamplerate/src)


# add_executable(dummy_backend dummy_backend.cpp  backend_infer.pb.cc backend_infer.grpc.pb.cc)
add_executable(backend_server
        backend_server.cpp
        backend_implement.cpp
        backend_infer.pb.cc
        backend_infer.grpc.pb.cc
        TFUtils.cpp
        nnet_data.cpp
        lpcnet.cpp nnet.cpp  freq.cpp
        kiss_fft.cpp pitch.cpp celt_lpc.cpp vec_avx.h)

add_executable(pcm2wav pcm2wav6.cpp wav_utils.cpp)

target_compile_definitions(backend_server PRIVATE __AVX__=1 TACOTRON2=1)

target_compile_options(backend_server
        PRIVATE "-mavx2"
        PRIVATE "-mfma"
        PRIVATE "-mavx"
        PRIVATE "-mf16c")

target_compile_features(backend_server PUBLIC cxx_std_11)
target_compile_features(pcm2wav PUBLIC cxx_std_11)

# target_link_libraries(dummy_backend protobuf grpc grpc++)
target_link_libraries(backend_server protobuf grpc grpc++ tensorflow m)

target_link_libraries(pcm2wav samplerate)
