cmake_minimum_required(VERSION 3.8)

project(lpctron_infer CXX)
link_directories(../tensorflow-1.15.0/lib)

set(LPCNET_INFER3_FILES lpcnet_infer3.cpp lpcnet.cpp nnet.cpp nnet_data.c freq.cpp
        kiss_fft.c pitch.cpp celt_lpc.cpp vec_avx.h)

# set(MINI_MODEL_FILES mini_model.cpp TFUtils.cpp)

add_executable(lpcnet_infer3 ${LPCNET_INFER3_FILES})

# add_executable(mini_model ${MINI_MODEL_FILES})

add_executable(taco_infer_cpp_2 taco_infer_cpp_2.cpp)
add_executable(taco_infer_cpp_0 taco_infer_cpp_0.cpp TFUtils.cpp tf_utils.cpp tf_diagnosis.cpp)

# add_executable(test_json test_json.cpp)

target_compile_definitions(lpcnet_infer3 PRIVATE __AVX__=1)

target_compile_options(lpcnet_infer3
        PRIVATE "-mavx2"
        PRIVATE "-mfma"
        PRIVATE "-mavx"
        PRIVATE "-mf16c")

target_link_libraries(lpcnet_infer3 m)

target_compile_features(lpcnet_infer3 PUBLIC cxx_std_11)
#target_compile_features(mini_model PUBLIC cxx_std_11)
target_compile_features(taco_infer_cpp_0 PUBLIC cxx_std_11)
target_compile_features(taco_infer_cpp_2 PUBLIC cxx_std_11)

# target_compile_features(test_json PUBLIC cxx_std_11)
# target_include_directories(test_json PUBLIC ../thirdparty/nlohmann_json/include)

# target_include_directories(mini_model PUBLIC ../tensorflow-1.15.0/include)
# target_link_libraries(mini_model tensorflow)

target_include_directories(taco_infer_cpp_0 PUBLIC ../tensorflow-1.15.0/include)
target_link_libraries(taco_infer_cpp_0 tensorflow)

# target_include_directories(taco_infer_cpp_2 PUBLIC ../tensorflow-1.15.0/include)
target_link_libraries(taco_infer_cpp_2 tensorflow)