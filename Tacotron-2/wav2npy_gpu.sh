#wav->s16(pcm)->to npy as meta[0] audio file
#audio-LJ024-0006.npy 

TRIM_WAV_DIR=../dataset/biaobei/trim_wav

PCM_DIR=../dataset/biaobei/pcms
F32_DIR=../dataset/biaobei/f32

FINAL_OUT_DIR=../dataset/training_data/audio

rm -rf $PCM_DIR
rm -rf $F32_DIR

#convert to PCM ( header removal)
mkdir -p $PCM_DIR

for i in ${TRIM_WAV_DIR}/*.wav
do
  s16_file=${PCM_DIR}/audio-$(basename "$i" | cut -d. -f1).s16
  echo $s16_file
  sox $i -r 22050 -c 1 -t sw - > $s16_file
done

# PCM(S16) to F32/NPY
mkdir -p ${F32_DIR}
for i in ${PCM_DIR}/*.s16
do
  f32_file=${F32_DIR}/$(basename "$i" | cut -d. -f1).npy
  echo $f32_file
  ../LPCNet/dump_data -test $i $f32_file
done

#Original Tacotron Training is done with Text and Audio. But when we integrate with LPCNET training has to done with Text and F32/NPY
#so that during Inference we input Text , we can F32/NPY from Tacotron and this can be fed to LPCNet Vocoder to Generate PCM.
#So replace All files in Audio with NPY/F32 Files.
rm -rf ${FINAL_OUT_DIR}
mkdir -p ${FINAL_OUT_DIR}

cp ${F32_DIR}/* ${FINAL_OUT_DIR}/.