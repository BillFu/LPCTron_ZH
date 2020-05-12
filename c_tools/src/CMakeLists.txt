cmake_minimum_required(VERSION 3.8)

project(dump_data C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES dump_data.c freq.c kiss_fft.c pitch.c celt_lpc.c)

set(MINI_SOURCE_FILES dump_data_mini.c freq.c kiss_fft.c pitch.c celt_lpc.c)

set(LPCNET_INFER_FILES lpcnet_infer.c lpcnet.c nnet.c nnet_data.c freq.c
        kiss_fft.c pitch.c celt_lpc.c vec_avx.h)

set(LPCNET_INFER2_FILES lpcnet_infer2.c lpcnet.c nnet.c nnet_data.c freq.c
        kiss_fft.c pitch.c celt_lpc.c vec_avx.h)

# add_definitions(-DTACOTRON2)

add_executable(dump_data_t2 ${SOURCE_FILES})
add_executable(dump_data_lpcnet ${SOURCE_FILES})
add_executable(dump_data_lpcnet_mini ${MINI_SOURCE_FILES})
add_executable(lpcnet_infer ${LPCNET_INFER_FILES})
add_executable(lpcnet_infer2 ${LPCNET_INFER2_FILES})

add_executable(test_json test_json.cpp)

target_compile_definitions(dump_data_t2 PRIVATE TACOTRON2=1)
target_compile_definitions(dump_data_lpcnet PRIVATE TACOTRON2=0)
target_compile_definitions(dump_data_lpcnet_mini PRIVATE TACOTRON2=0)
target_compile_definitions(lpcnet_infer PRIVATE __AVX__=1 TACOTRON2=1)
target_compile_definitions(lpcnet_infer2 PRIVATE __AVX__=1 TACOTRON2=1)

target_compile_options(lpcnet_infer
        PRIVATE "-mavx2"
        PRIVATE "-mfma"
        PRIVATE "-mavx"
        PRIVATE "-mf16c")

target_compile_options(lpcnet_infer2
        PRIVATE "-mavx2"
        PRIVATE "-mfma"
        PRIVATE "-mavx"
        PRIVATE "-mf16c")


target_link_libraries(dump_data_t2 m)
target_link_libraries(dump_data_lpcnet m)
target_link_libraries(dump_data_lpcnet_mini m)
target_link_libraries(lpcnet_infer m)
target_link_libraries(lpcnet_infer2 m)

target_include_directories(test_json PUBLIC ../thirdparty/nlohmann_json/include)
