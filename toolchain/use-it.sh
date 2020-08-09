#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PATH="$DIR/local/bin:$PATH"
export TOOLCHAIN="$DIR"
export SYSROOT="$DIR/../build/sysroot"

export SKIFT_TOOLCHAIN="$DIR"
export SKIFT_SYSROOT="$DIR/../build/sysroot"
export SKIFT_CONTRIBROOT="$DIR/../contrib"

echo "$DIR/local/bin"
