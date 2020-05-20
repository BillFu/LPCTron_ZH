cmake_minimum_required(VERSION 3.8)

project(lpctron_infer CXX)

link_directories(../tf_c_api-1.15.0/lib)
include_directories(../tf_c_api-1.15.0/include)

add_executable(taco_infer_c_api taco_infer_c_api.cpp TFUtils.cpp)
add_executable(backend_infer backend_infer.cpp TFUtils.cpp
        lpcnet.cpp nnet.cpp nnet_data.c freq.cpp
        kiss_fft.c pitch.cpp celt_lpc.cpp vec_avx.h)

target_compile_definitions(backend_infer PRIVATE __AVX__=1 TACOTRON2=1)

target_compile_options(backend_infer
        PRIVATE "-mavx2"
        PRIVATE "-mfma"
        PRIVATE "-mavx"
        PRIVATE "-mf16c")

target_compile_features(taco_infer_c_api PUBLIC cxx_std_11)
target_compile_features(backend_infer PUBLIC cxx_std_11)

target_link_libraries(taco_infer_c_api tensorflow)
target_link_libraries(backend_infer tensorflow m)
