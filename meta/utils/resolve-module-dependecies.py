#!/bin/env python3
import sys

def panic(msg):
    print(msg, file=sys.stderr)
    sys.exit(-1)

def log(msg):
    print(msg, file=sys.stderr)

file_name = sys.argv[1]
global_modulemap = sys.argv[2]

modules = {}

with open(global_modulemap) as f:
    lines = f.readlines()

    for line in lines:
        line = line.strip()
        modules[line.split(" ")[0]] = line.split(" ")[1]

with open(file_name) as f:
    lines = f.readlines()

    for line in lines:
        line = line.strip()

        if line.startswith("import \""):
            print(modules[line.replace("import \"", "").replace(
                "\"", "")] + " ", end='')

        elif line.startswith("import <"):
            print(modules[line.replace("import <", "").replace(
                ">", "")] + " ", end='')

        elif line.startswith("import "):
            print(modules[line.replace("import ", "").replace(
                ";", "")] + " ", end='')

print("")  # end line
