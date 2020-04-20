#!/usr/bin/env bash

rm -f nohup.out

nohup sh train_lpcnet.sh &
echo "see console output, tail -f nohup.out"