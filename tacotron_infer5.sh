#!/usr/bin/env bash

mkdir -p tacotron_output

# --input_text_file=input_text/input_text.txt \

python3.6 -m Tacotron-2.tacotron_infer5 \
--model_file=tacotron_model/tacotron.pb \
--output_dir=lpcnet_test
