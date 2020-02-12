#wav->s16(pcm)->to npy as meta[0] audio file
#audio-LJ024-0006.npy 

#convert to PCM ( header removal)
mkdir -p ../dataset/biaobei/pcms

for i in /mnt/e/BZNSYP/Wave/*.wav
do
  s16_file=../dataset/biaobei/pcms/audio-$(basename "$i" | cut -d. -f1).s16
  echo $s16_file
  sox $i -r 22050 -c 1 -t sw - > $s16_file
done

# Utility to convert PCM to F32/NPY
# cd ../LPCNet
# (dump_data compileted with taco=1)
# make clean && make dump_data taco=1
# cd -

# PCM(S16) to F32/NPY
# mkdir -p ../dataset/LJSpeech-1.1/f32
# for i in ../dataset/LJSpeech-1.1/pcms/*.s16
# do
# ../LPCNet/dump_data -test $i ../dataset/LJSpeech-1.1/f32/$(basename "$i" | cut -d. -f1).npy
# done

#Original Tacotron Training is done with Text and Audio. But when we integrate with LPCNET training has to done with Text and F32/NPY
#so that during Inference we input Text , we can F32/NPY from Tacotron and this can be fed to LPCNet Vocoder to Generate PCM.
#So replace All files in Audio with NPY/F32 Files.
# cp ../dataset/LJSpeech-1.1/f32/* ../dataset/training_data/audio/.