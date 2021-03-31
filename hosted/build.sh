#!/bin/bash
g++ \
    -g \
    -std=c++20 \
    -Ilibraries \
    -Iapps \
    -Ilibraries/libc \
    -I. \
    -D__CONFIG_IS_RELEASE__=1 \
    -DDISABLE_LOGGER \
    -fsanitize=address \
    -fsanitize=undefined \
    hosted/test.cpp \
    libraries/libgraphic/png/PngReader.cpp \
    libraries/libcompression/Inflate.cpp \
    libraries/libcompression/CRC.cpp
