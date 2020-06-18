
TACOTRON_MODEL_FILE=tacotron.pb
OUT_WAV_DIR=out_wav

mkdir -p $OUT_WAV_DIR

chmod +x backend_infer/bin/release/backend_server

valgrind -v backend_infer/bin/release/backend_server $TACOTRON_MODEL_FILE $OUT_WAV_DIR
