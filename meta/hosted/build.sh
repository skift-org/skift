#!/bin/bash
g++ \
    -g \
    -std=c++20 \
    -Imeta/hosted/includes \
    -Iuserspace/libs \
    -Iuserspace/apps \
    -Iuserspace/hosted/includes \
    -D__CONFIG_IS_RELEASE__=0 \
    -D__CONFIG_IS_HOSTED__=1 \
    -DDISABLE_LOGGER \
    meta/hosted/test.cpp \
    userspace/libs/libpng/Reader.cpp \
    userspace/libs/libcompression/Inflate.cpp \
    userspace/libs/libcompression/CRC.cpp \
    userspace/libs/libio/File.cpp \
    userspace/libs/libio/Format.cpp \
    userspace/libs/libio/Streams.cpp \
    meta/hosted/plugs/*.cpp \
    -fsanitize=address \
    -fsanitize=undefined
