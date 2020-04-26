

ffmpeg -f s16le -ar 16k -ac 1 -i synth_pcm1.f16 -ar 16k synth_16k_1.wav

ffmpeg -f s16le -ar 16k  -ac 1 -i synth_pcm1.f16 -ar 8k synth_8k_1.wav