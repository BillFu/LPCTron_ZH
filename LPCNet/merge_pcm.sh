#!/bin/bash

# in_pcm_files=../dataset/biaobei/pcms/*.s16

if [ $# -eq 2 ]; then
	echo "OK! Your command line have contained 2 arguments."
else
    echo "correct usage: merge_pcm.sh num_input_files out_pcm_file"
    exit 0
fi

count=$1
out_file_name=$2

for i in ../dataset/biaobei/pcms/*.s16
do
    cat "$i" >> $out_file_name
    echo $i

	((count=count-1))

    if [ "$count" -eq 0 ];then
    	echo "I'am goint to break!"
    	break
	fi
done

echo "$out_file_name created..."
