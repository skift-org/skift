#!/bin/bash
g++ \
    -g \
    -std=c++20 \
    -Imeta/hosted/includes \
    -Iuserspace/libraries \
    -Iuserspace/apps \
    -Iuserspace/hosted/includes \
    -D__CONFIG_IS_RELEASE__=0 \
    -D__CONFIG_IS_HOSTED__=1 \
    -DDISABLE_LOGGER \
    meta/hosted/test.cpp \
    userspace/libraries/libpng/Reader.cpp \
    userspace/libraries/libcompression/Inflate.cpp \
    userspace/libraries/libcompression/CRC.cpp \
    userspace/libraries/libio/File.cpp \
    userspace/libraries/libio/Format.cpp \
    userspace/libraries/libio/Streams.cpp \
    meta/hosted/plugs/*.cpp \
    -fsanitize=address \
    -fsanitize=undefined
