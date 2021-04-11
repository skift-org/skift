#!/bin/sh

if [[ -z $1 && -z $2 ]]; then
    echo "Please specify input and output file."
    exit 1
fi

ffmpeg -i $1 -f s16le -acodec pcm_s16le -ar 96000 -ac 1 $2
