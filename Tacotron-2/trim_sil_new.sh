OUT_DIR=../dataset/biaobei/trim_wav

mkdir -p ${OUT_DIR}

python3 trim_sil_new.py \
--in_wav_dir=/Users/bill/BZNSYP/wave \
--in_sr=16000 \
--out_wav_dir=${OUT_DIR}