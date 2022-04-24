#!/usr/bin/env bash
clang++ $(cat compile_flags.txt) -o main.out main.cpp && ./main.out
