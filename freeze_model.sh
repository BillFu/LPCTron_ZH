#!/usr/bin/env bash

python3.6 -m Tacotron-2.freeze_model \
--checkpoint_dir=taco_pretrained \
--out_pb_dir=tacotron_model \
--out_pb_file=tacotron.pb