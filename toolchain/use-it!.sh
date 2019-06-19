#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PATH="$DIR/local/bin:$PATH"
export TOOLCHAIN="$DIR"
export SYSROOT="$DIR/../build/sysroot"

echo "$DIR/local/bin"