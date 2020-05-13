
# chmod a+x dump_data_lpcnet
# ./dump_data_lpcnet -test audio-000001.s16 features1.f32

chmod a+x lpcnet_infer

./lpcnet_infer feature_1003.f32 synth_pcm4.f16

ffmpeg -f s16le -ar 16k -ac 1 -i synth_pcm4.f16 -ar 16k synth_16k_4.wav
