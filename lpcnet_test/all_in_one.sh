
# chmod a+x dump_data_lpcnet
# ./dump_data_lpcnet -test audio-000001.s16 features1.f32

chmod a+x lpcnet_infer
#./lpcnet_infer features1.f32 synth_pcm1.f16

./lpcnet_infer feature_1002.f32 synth_pcm2.f16

ffmpeg -f s16le -ar 16k -ac 1 -i synth_pcm2.f16 -ar 16k synth_16k_2.wav

