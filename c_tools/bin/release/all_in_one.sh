
# chmod a+x dump_data_lpcnet
# ./dump_data_lpcnet -test audio-000001.s16 features1.f32

chmod a+x lpcnet_infer

chmod a+x taco_infer_c_api

./taco_infer_c_api 1008_ipa.txt tacotron.pb 1008_feature.dat

./lpcnet_infer 1008_feature.dat 1008_pcm.f16

ffmpeg -f s16le -ar 16k -ac 1 -i 1008_pcm.f16 -ar 16k 1008_16k_5.wav
