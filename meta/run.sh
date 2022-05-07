#!/usr/bin/env bash
mkdir -p bin

export LC_ALL=C

echo
echo --- Clang ---
clang++ $(cat compile_flags.txt) -o bin/$1.out $1.cpp && ./bin/$1.out

echo
echo ---- Gcc ----
g++ $(cat compile_flags.txt) -o bin/$1.out $1.cpp && ./bin/$1.out

echo
echo ----------------------------
echo
date
echo
echo "Command exited with code $?"
echo
