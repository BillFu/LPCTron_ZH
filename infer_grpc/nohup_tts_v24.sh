#!/usr/bin/env bash

rm -f nohup_tts.out

nohup sh tts_v24.sh &> nohup_tts.out&
echo "see console output, tail -f nohup_tts.out"