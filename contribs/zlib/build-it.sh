#!/bin/sh

cd sources
CROSS_PREFIX=i686-pc-skift- ./configure && make libz.a
cd ..
