
TACOTRON_MODEL_FILE=tacotron.pb
OUT_PCM_DIR=out_pcm
OUT_WAV_DIR=out_wav

mkdir -p $OUT_PCM_DIR
mkdir -p $OUT_WAV_DIR


chmod +x backend_infer/bin/release/backend_server

backend_infer/bin/release/backend_server $TACOTRON_MODEL_FILE $OUT_PCM_DIR $OUT_WAV_DIR
