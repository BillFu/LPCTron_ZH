
# chmod a+x dump_data_lpcnet
# ./dump_data_lpcnet -test audio-000001.s16 features1.f32

chmod a+x lpcnet_infer
./lpcnet_infer features1.f32 synth_pcm1.f16

#./lpcnet_infer feature_1001.f32 synth_pcm1.f16

ffmpeg -f s16le -ar 16k -ac 1 -i synth_pcm1.f16 -ar 16k synth_16k_1.wav
ffmpeg -f s16le -ar 16k -ac 1 -i synth_pcm1.f16 -ar 8k  synth_8k_1.wav

