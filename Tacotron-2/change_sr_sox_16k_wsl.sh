#!/bin/bash

INPUT_WAV_DIR=/mnt/e/BZNSYP/Wave
OUTPUT_WAV_DIR=/mnt/e/BZNSYP/Wave_16k
NEW_SR=16000

mkdir -p ${OUTPUT_WAV_DIR}

echo "new sampled *.wav files will be saved at ./${OUTPUT_WAV_DIR}"
for in_wav_file in ${INPUT_WAV_DIR}/*.wav; do
    # echo $in_wav_file
    basename="${in_wav_file##*/}"
    new_name=${OUTPUT_WAV_DIR}/${basename}
    echo ${new_name}
    sox  ${in_wav_file} -r ${NEW_SR} ${new_name}
done
