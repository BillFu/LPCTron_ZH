
F32_DIR=../dataset/biaobei/f32
FINAL_OUT_DIR=../dataset/training_data/audio

find ${F32_DIR}/ -name "*.npy" -exec cp {} ${FINAL_OUT_DIR}/ \;
