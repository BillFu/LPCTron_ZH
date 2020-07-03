
# TACOTRON_MODEL_FILE=tacotron.pb
# OUT_WAV_DIR=out_wav

# mkdir -p $OUT_WAV_DIR

SERVER_EXE=backend/bin/backend_server
chmod +x $SERVER_EXE

# backend_infer/bin/backend_server $TACOTRON_MODEL_FILE $OUT_WAV_DIR

$SERVER_EXE 
