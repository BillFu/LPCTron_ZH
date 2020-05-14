cmake_minimum_required(VERSION 3.8)

project(lpcnet_infer CXX)

set(LPCNET_INFER3_FILES lpcnet_infer3.cpp lpcnet.cpp nnet.cpp nnet_data.c freq.cpp
        kiss_fft.c pitch.cpp celt_lpc.cpp vec_avx.h)

add_executable(lpcnet_infer3 ${LPCNET_INFER3_FILES})

# add_executable(test_json test_json.cpp)

target_compile_definitions(lpcnet_infer3 PRIVATE __AVX__=1)

target_compile_options(lpcnet_infer3
        PRIVATE "-mavx2"
        PRIVATE "-mfma"
        PRIVATE "-mavx"
        PRIVATE "-mf16c")

target_link_libraries(lpcnet_infer3 m)

target_compile_features(lpcnet_infer3 PUBLIC cxx_std_11)

# target_compile_features(test_json PUBLIC cxx_std_11)
# target_include_directories(test_json PUBLIC ../thirdparty/nlohmann_json/include)

