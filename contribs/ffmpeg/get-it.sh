#!/bin/sh
VERSION=4.3

echo "Getting FFMPEG version: $VERSION"
git clone --depth 1 --branch release/$VERSION https://git.ffmpeg.org/ffmpeg.git sources