#!/bin/env python3
import sys

src_dir = sys.argv[1]
dst_dir = sys.argv[2]
files = sys.argv[3:]


def panic(msg):
    print(msg, file=sys.stderr)
    sys.exit(-1)


def log(msg):
    print(msg, file=sys.stderr)


def find_module_export(file):
    for line in file.readlines():
        line = line.strip()
        if line.startswith("export module ") or line.startswith("module "):
            return line

    return None


for file_name in files:
    if file_name.endswith(".h"):
        module_name = file_name.replace(src_dir, "")
        module_path = (dst_dir + file_name.replace(src_dir, "")) \
            .replace(".h", ".pch")
        print(f"{module_name} {module_path}")

    elif file_name.endswith(".cppm"):
        with open(file_name) as f:
            module_export = find_module_export(f)

            if module_export is None:
                continue

            module_name = module_export \
                .replace("export module ", "") \
                .replace("module ", "") \
                .replace(";", "")

            module_path = (dst_dir + file_name.replace(src_dir, "")) \
                .replace(".cppm", ".gmc")

            print(f"{module_name} {module_path}")
    else:
        log(f"File type not supported {file_name}")
