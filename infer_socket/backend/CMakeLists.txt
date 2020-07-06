cmake_minimum_required(VERSION 3.16)
project(backend)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

add_executable(backend_server backend_server2.cpp
        BackendImp.cpp TFUtils.cpp
        nnet_data.cpp lpcnet.cpp nnet.cpp  freq.cpp
        kiss_fft.cpp pitch.cpp celt_lpc.cpp vec_avx.h)
set_target_properties(backend_server PROPERTIES CXX_STANDARD 14)


target_compile_definitions(backend_server PRIVATE __AVX__=1 TACOTRON2=1)

target_compile_options(backend_server
        PRIVATE "-mavx2"
        PRIVATE "-mfma"
        PRIVATE "-mavx"
        PRIVATE "-mf16c")

target_link_libraries(backend_server tensorflow samplerate
        event event_core event_pthreads event_extra
        boost_thread  boost_chrono-mt)
