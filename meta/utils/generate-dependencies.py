#!/bin/env python3
import sys


def panic(msg):
    print(msg, file=sys.stderr)
    sys.exit(-1)


def log(msg):
    print(msg, file=sys.stderr)


src_dir = sys.argv[1]
dst_dir = sys.argv[2]
file_names = sys.argv[3:-1]
global_modulemap = sys.argv[-1]

modules = {}

with open(global_modulemap) as f:
    lines = f.readlines()

    for line in lines:
        line = line.strip()
        modules[line.split(" ")[0]] = line.split(" ")[1]


for file_name in file_names:
    with open(file_name) as f:
        lines = f.readlines()

        target_name = ""

        if file_name.endswith(".cpp"):
            target_name = file_name.replace(
                src_dir, dst_dir).replace(".cpp", ".o")
        elif file_name.endswith(".h"):
            target_name = file_name
        else:
            panic(f"Unreconized file type: {file_name}")

        print(target_name + ":", end=' ')

        for line in lines:
            line = line.strip()

            module_name = ""

            if line.startswith("import \""):
                module_name = line.replace("import \"", "").replace("\"", "")

            elif line.startswith("import <"):
                module_name = line.replace("import <", "").replace(">", "")

            elif line.startswith("import "):
                module_name = line.replace("import ", "").replace(";", "")

            if module_name != "":
                resolved_module_name = modules[module_name]
                resolved_module_name = resolved_module_name.replace(
                    ".gmc", ".module.o")
                print(resolved_module_name, end=' ')

        print("")  # end line
