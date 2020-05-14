#!/usr/bin/env bash

# mkdir -p frontend_output

python3.6 -m Tacotron-2.frontend_infer \
--input_sentence="抱歉，我的账户只剩三元，而我的智商欠费却为一百五十二元。" \
--sentence_id=1008 \
--output_dir=lpcnet_test
