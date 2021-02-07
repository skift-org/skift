#!/bin/sh

cd sources
sysroot="$PWD/../../build/sysroot"
export CPPFLAGS="-I$sysroot/System/Includes"
export LDFLAGS="-L$sysroot/System/Libraries -static -lc"
./configure \
 --enable-cross-compile --target-os=minix \
 --cross_prefix=i686-pc-skift- --arch=i686 --prefix=./install && make -j $(nproc)
cd ..
