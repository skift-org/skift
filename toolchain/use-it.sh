#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PATH="$DIR/local/bin:$PATH"

export SKIFT_SOURCEROOT="$DIR/.."
export SKIFT_TOOLCHAIN="$SKIFT_SOURCEROOT/toolchain"
export SKIFT_SYSROOT="$SKIFT_SOURCEROOT/sysroot"
export SKIFT_CONTRIBROOT="$SKIFT_SOURCEROOT/contributions"

echo "$DIR/local/bin"
