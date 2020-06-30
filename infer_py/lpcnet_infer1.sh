#!/usr/bin/env bash

mkdir -p output
mkdir -p output/out_pcm
mkdir -p output/out_wav

python3.6 -m lpcnet.lpcnet_infer \
--lpcnet_model_file=lpcnet_model/model_loss-2.961_119_.hdf5 \
--out_pcm_dir=output/out_pcm \
--out_wav_dir=output/out_wav
