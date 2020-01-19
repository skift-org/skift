#!/bin/bash
set -e

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

source "$DIR/use-it!.sh"

pushd tarballs
    if [ ! -e "binutils-2.31.tar.gz" ]; then
        wget "http://ftp.gnu.org/gnu/binutils/binutils-2.31.tar.gz"
    else
        echo "Skipped downloading binutils"
    fi

    if [ ! -e "gcc-8.2.0.tar.gz" ]; then
        wget "http://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz"
    else
        echo "Skipped downloading gcc"
    fi

    if [ ! -d "binutils-2.31" ]; then
        echo "Extracting binutils..."
        tar -xf "binutils-2.31.tar.gz"

        pushd "binutils-2.31"
            patch -p1 < $DIR/patches/binutils.patch
        popd
    else
        echo "Skipped extracting binutils"
    fi

    if [ ! -d "gcc-8.2.0" ]; then
        echo "Extracting gcc..."
        tar -xf "gcc-8.2.0.tar.gz"

        pushd "gcc-8.2.0"
            patch -p1 < $DIR/patches/gcc.patch
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
        $DIR/tarballs/binutils-2.31/configure --target=$TARGET \
                                              --prefix=$PREFIX \
                                              --with-sysroot=$SYSROOT \
                                              --disable-werror || exit 1
        make -j $MAKEJOBS || exit 1
        make install || exit 1
    popd

    pushd gcc
        $DIR/tarballs/gcc-8.2.0/configure --target=$TARGET \
                                          --prefix=$PREFIX \
                                          --disable-nls \
                                          --with-newlib \
                                          --with-sysroot=$SYSROOT \
                                          --enable-languages=c|| exit 1

        make -j $MAKEJOBS all-gcc all-target-libgcc || exit 1
        make install-gcc install-target-libgcc || exit 1
    popd
popd

touch $PREFIX/build-ok
