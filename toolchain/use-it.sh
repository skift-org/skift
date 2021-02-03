#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PATH="$DIR/local/bin:$PATH"

export SKIFT_SOURCEROOT="$DIR/.."
export SKIFT_TOOLCHAIN="$SKIFT_SOURCEROOT/toolchain"
export SKIFT_SYSROOT="$SKIFT_SOURCEROOT/build/sysroot"
export SKIFT_CONTRIBROOT="$SKIFT_SOURCEROOT/contribs"

echo "$DIR/local/bin"
