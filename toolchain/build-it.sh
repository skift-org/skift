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

PREFIX="$DIR/local"
SYSROOT="$DIR/../build/sysroot"

if [ -e "$PREFIX/build-ok" ]; then
    echo "The toolchain is built and ready :)"
    exit 0
fi

DEV_MODE=
while [ "$1" != "" ]; do
    case $1 in
        --dev )           DEV_MODE=1
                          ;;
    esac
    shift
done

cd "$DIR"

mkdir -p tarballs

source "$DIR/use-it.sh"

# Download and unpack GCC and binutils
# ---------------------------------------------------------------------------- #

pushd tarballs
    if [ ! -e "$BINUTILS_FILENAME" ]; then
        wget $WGETFLAGS "$BINUTILS_URL"
    else
        echo "Skipped downloading binutils"
    fi

    if [ ! -e "$GCC_FILENAME" ]; then
        wget $WGETFLAGS "$GCC_URL"
    else
        echo "Skipped downloading gcc"
    fi

    if [ ! -d "$BINUTILS_DIRECTORY" ]; then
        echo "Extracting binutils..."
        tar -xf "$BINUTILS_FILENAME"

        pushd "$BINUTILS_DIRECTORY"
            if [ "$DEV_MODE" = "1" ]; then
                git init
                git add .
                git commit -am "BASE"
                git apply $DIR/patches/binutils.patch
            else
                patch -p1 < $DIR/patches/binutils.patch
            fi
        popd
    else
        echo "Skipped extracting binutils"
    fi

    if [ ! -d "$GCC_DIRECTORY" ]; then
        echo "Extracting gcc..."
        tar -xf "$GCC_FILENAME"

        pushd "$GCC_DIRECTORY"
            if [ "$DEV_MODE" = "1" ]; then
                git init
                git add .
                git commit -am "BASE"
                git apply $DIR/patches/gcc.patch
            else
                patch -p1 < $DIR/patches/gcc.patch
            fi
        popd
    else
        echo "Skipped extracting gcc"
    fi
popd

mkdir -p $PREFIX

if [ -z "$MAKEJOBS" ]; then
    MAKEJOBS=$(nproc)
fi

# Build GCC and binutils for the x86_32 target
# ---------------------------------------------------------------------------- #

TARGET32=i686-pc-skift

mkdir -p "$DIR/build-x86_32/binutils"
mkdir -p "$DIR/build-x86_32/gcc"

pushd "$DIR/build-x86_32/"
    unset PKG_CONFIG_LIBDIR # Just in case

    pushd binutils
        "$DIR/tarballs/$BINUTILS_DIRECTORY/configure" \
            --target=$TARGET32 \
            --prefix=$PREFIX \
            --with-sysroot=$SYSROOT \
            --disable-werror || exit 1

        make -j $MAKEJOBS || exit 1
        make install || exit 1
    popd

    pushd gcc
        "$DIR/tarballs/$GCC_DIRECTORY/configure" \
            --target=$TARGET32 \
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

# Build GCC and binutils for the x86_64 target
# ---------------------------------------------------------------------------- #

TARGET64=x86_64-pc-skift

mkdir -p "$DIR/build-x86_64/binutils"
mkdir -p "$DIR/build-x86_64/gcc"

pushd "$DIR/build-x86_64/"
    unset PKG_CONFIG_LIBDIR # Just in case

    pushd binutils
        "$DIR/tarballs/$BINUTILS_DIRECTORY/configure" \
            --target=$TARGET64 \
            --prefix=$PREFIX \
            --with-sysroot=$SYSROOT \
            --disable-werror || exit 1

        make -j $MAKEJOBS || exit 1
        make install || exit 1
    popd

    pushd gcc
        "$DIR/tarballs/$GCC_DIRECTORY/configure" \
            --target=$TARGET64 \
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
