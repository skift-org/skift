#!/usr/bin/env bash
mkdir -p bin

clang++ $(cat compile_flags.txt) -o bin/$1.out $1.cpp && ./bin/$1.out
