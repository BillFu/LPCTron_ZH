#!/usr/bin/env bash

mkdir -p tacotron_output

python3.6 -m Tacotron-2.tacotron_infer2 \
--input_text_file=input_text/input_text.txt \
--checkpoint_dir=taco_pretrained \
--output_dir=tacotron_output
