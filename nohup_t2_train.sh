#!/usr/bin/env bash

rm -rf nohup.ou

nohup sh t2_train.sh &
echo "see console output, tail -f nohup.out"
