F32_DIR=../dataset/biaobei/f32
AUDIO_DIR=../dataset/training_data/audio  # save final f32/npy data

rm -rf $AUDIO_DIR
mkdir -p $AUDIO_DIR

cp $F32_DIR/* ${AUDIO_DIR}/.
