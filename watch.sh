#!/usr/bin/env bash
mkdir -p bin

while [ : ]
do
clang++ -O3 -S $(cat compile_flags.txt) $1.cpp -o - 2> bin/err.txt | c++filt > bin/code.asm
./run.sh $1 2> /dev/null > bin/out.txt
sleep 3
done
