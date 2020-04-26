

ffmpeg -f s16le -ar 16k -ac 1 -i audio-000001.s16 -ar 16k synth_16k_1.wav

ffmpeg -f s16le -ar 16k  -ac 1 -i audio-000001.s16 -ar 8k synth_8k_1.wav