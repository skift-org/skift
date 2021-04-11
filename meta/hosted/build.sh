#!/bin/bash
g++ \
    -g \
    -std=c++20 \
    -Ilibraries \
    -Iapps \
    -Ihosted/includes \
    -I. \
    -D__CONFIG_IS_RELEASE__=0 \
    -D__CONFIG_IS_HOSTED__=1 \
    -DDISABLE_LOGGER \
    hosted/test.cpp \
    libraries/libgraphic/png/PngReader.cpp \
    libraries/libcompression/Inflate.cpp \
    libraries/libcompression/CRC.cpp \
    libraries/libio/File.cpp \
    libraries/libio/Format.cpp \
    hosted/plugs/*.cpp \
    # -fsanitize=address \
    # -fsanitize=undefined \
