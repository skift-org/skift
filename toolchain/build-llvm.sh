#!/bin/bash
set -e

# ----- Configs -------------------------------------------------------------- #

LLVM_VERSION=11.0.1
LLVM_DIRECTORY="llvm-$LLVM_VERSION.src"
LLVM_FILENAME="llvm-$LLVM_VERSION.src.tar.xz"
LLVM_URL=https://github.com/llvm/llvm-project/releases/download/llvmorg-$LLVM_VERSION/$LLVM_FILENAME

CLANG_FILENAME="clang-$LLVM_VERSION.src.tar.xz"
CLANG_DIRECTORY="$LLVM_DIRECTORY/tools"
CLANG_URL=https://github.com/llvm/llvm-project/releases/download/llvmorg-$LLVM_VERSION/$CLANG_FILENAME

# ---------------------------------------------------------------------------- #

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PREFIX="$DIR/local"
SYSROOT="$DIR/../build/sysroot"

if [ -e "$PREFIX/build-ok" ]; then
    echo "The toolchain is built and ready :)"
    exit 0
fi

cd "$DIR"

mkdir -p tarballs

source "$DIR/use-it.sh"

# Download and unpack LLVM and Clang
# ---------------------------------------------------------------------------- #

pushd tarballs
    if [ ! -e "$LLVM_FILENAME" ]; then
        wget $WGETFLAGS "$LLVM_URL"
    else
        echo "Skipped downloading llvm"
    fi

    if [ ! -e "$CLANG_FILENAME" ]; then
        wget $WGETFLAGS "$CLANG_URL"
    else
        echo "Skipped downloading clang"
    fi

    if [ ! -d "$LLVM_DIRECTORY" ]; then
        echo "Extracting llvm..."
        tar -xf "$LLVM_FILENAME"
    else
        echo "Skipped extracting llvm"
    fi

    if [ ! -d "$CLANG_DIRECTORY" ]; then
        echo "Extracting clang..."
        tar -xf "$CLANG_FILENAME"
    else
        echo "Skipped extracting clang"
    fi
popd

mkdir -p $PREFIX

if [ -z "$MAKEJOBS" ]; then
    MAKEJOBS=$(nproc)
fi

# Build LLVM and clang
# ---------------------------------------------------------------------------- #

mkdir -p "$DIR/build-llvm/llvm"

pushd "$DIR/build-llvm/"
    unset PKG_CONFIG_LIBDIR # Just in case

    pushd llvm
        cmake "$DIR/tarballs/$LLVM_DIRECTORY"  -DCMAKE_INSTALL_PREFIX=$PREFIX -G Ninja

        cmake --build . || exit 1
        cmake --build . --target install || exit 1
    popd

    pushd clang

    popd
popd

touch $PREFIX/build-ok
