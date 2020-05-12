#!/usr/bin/env bash

mkdir -p tacotron_output

# --input_text_file=input_text/input_text.txt \

python3.6 -m Tacotron-2.tacotron_infer4 \
--checkpoint_dir=taco_pretrained \
--output_dir=tacotron_output
