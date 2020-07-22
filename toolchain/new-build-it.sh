#!/bin/bash
set -e

# ----- Configs -------------------------------------------------------------- #

BINUTILS_VERSION=2.33.1
BINUTILS_DIRECTORY="binutils-$BINUTILS_VERSION"
BINUTILS_FILENAME="$BINUTILS_DIRECTORY.tar.gz"
BINUTILS_URL="http://ftp.gnu.org/gnu/binutils/$BINUTILS_FILENAME"

GCC_VERSION=10.1.0
GCC_DIRECTORY="gcc-$GCC_VERSION"
GCC_FILENAME="$GCC_DIRECTORY.tar.gz"
GCC_URL="http://ftp.gnu.org/gnu/gcc/$GCC_DIRECTORY/$GCC_FILENAME"

# ---------------------------------------------------------------------------- #

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

TARGET=i686-pc-skift
PREFIX="$DIR/local"
SYSROOT="$DIR/../build/sysroot"

if [ -e "$PREFIX/build-ok" ]; then
    echo "The toolchain is build and ready :)"
    exit 0
fi

cd "$DIR"

mkdir -p tarballs

source "$DIR/use-it.sh"

pushd tarballs
    if [ ! -e "$BINUTILS_FILENAME" ]; then
        wget "$BINUTILS_URL"
    else
        echo "Skipped downloading binutils"
    fi

    if [ ! -e "$GCC_FILENAME" ]; then
        wget "$GCC_URL"
    else
        echo "Skipped downloading gcc"
    fi

    if [ ! -d "$BINUTILS_DIRECTORY" ]; then
        echo "Extracting binutils..."
        tar -xf "$BINUTILS_FILENAME"

        pushd "$BINUTILS_DIRECTORY"
            git init
            git add .
            git commit -am "BASE"
            git apply $DIR/patches/new-binutils.patch
        popd
    else
        echo "Skipped extracting binutils"
    fi

    if [ ! -d "$GCC_DIRECTORY" ]; then
        echo "Extracting gcc..."
        tar -xf "$GCC_FILENAME"

        pushd "$GCC_DIRECTORY"
            git init
            git add .
            git commit -am "BASE"
            git apply $DIR/patches/new-gcc.patch
        popd
    else
        echo "Skipped extracting gcc"
    fi
popd

mkdir -p $PREFIX

mkdir -p "$DIR/build/binutils"
mkdir -p "$DIR/build/gcc"

if [ -z "$MAKEJOBS" ]; then
    MAKEJOBS=$(nproc)
fi

pushd "$DIR/build/"
    unset PKG_CONFIG_LIBDIR # Just in case

    pushd binutils
        "$DIR/tarballs/$BINUTILS_DIRECTORY/configure" \
            --target=$TARGET \
            --prefix=$PREFIX \
            --with-sysroot=$SYSROOT \
            --disable-werror || exit 1

        make -j $MAKEJOBS || exit 1
        make install || exit 1
    popd

    pushd gcc
        "$DIR/tarballs/$GCC_DIRECTORY/configure" \
            --target=$TARGET \
            --prefix=$PREFIX \
            --disable-nls \
            --with-newlib \
            --with-sysroot=$SYSROOT \
            --enable-languages=c,c++|| exit 1

        make -C "$DIR/../" install-headers || exit 1

        make -j $MAKEJOBS all-gcc all-target-libgcc || exit 1
        make install-gcc install-target-libgcc || exit 1

        make all-target-libstdc++-v3 || exit 1
        make install-target-libstdc++-v3 || exit 1
    popd
popd

touch $PREFIX/build-ok
