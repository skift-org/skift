#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

$DIR/check-reqs.sh

TARGET=i686-elf
PREFIX="$DIR/local"

cd "$DIR"

mkdir -p tarballs

pushd tarballs
    if [ ! -e "binutils-2.31.tar.gz" ]; then
        wget "http://ftp.gnu.org/gnu/binutils/binutils-2.31.tar.gz"
    fi

    if [ ! -e "gcc-8.2.0.tar.gz" ]; then
        wget "http://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz"
    fi

    if [ ! -d "binutils-2.31" ]; then
        tar -xf "binutils-2.31.tar.gz"
    fi

    if [ ! -d "gcc-8.2.0" ]; then
        tar -xf "gcc-8.2.0.tar.gz"
    fi
popd

mkdir -p $PREFIX

mkdir -p "$DIR/build/binutils"
mkdir -p "$DIR/build/gcc"

pushd "$DIR/build/"

    unset PKG_CONFIG_LIBDIR # Just in case

    pushd binutils
        $DIR/tarballs/binutils-2.31/configure --target=$TARGET --prefix=$PREFIX --disable-werror || exit 1
        make -j $(nproc)
        make install
    popd

    pushd gcc
        $DIR/tarballs/gcc-8.2.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --disable-libssp --without-headers || exit 1
        make -j $(nproc) all-gcc all-target-libgcc
        make install-gcc install-target-libgcc
    popd
popd