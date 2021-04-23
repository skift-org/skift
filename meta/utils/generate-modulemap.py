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


for file_name in files:
    if file_name.endswith(".h"):
        module_name = file_name.replace(src_dir, "")
        module_path = (dst_dir + file_name.replace(src_dir, "")) \
            .replace(".h", ".pch")
        print(f"{module_name} {module_path}")

    elif file_name.endswith(".module.cpp"):
        with open(file_name) as f:
            first_line = f.readline().strip()

            while not (first_line.startswith("export module ") or first_line.startswith("module ")):
                first_line = f.readline().strip()

            if not first_line.startswith("export module "):
                if not first_line.startswith("module "):
                    panic("The file is not a valid module file")

            module_name = first_line \
                .replace("export module ", "") \
                .replace("module ", "") \
                .replace(";", "")

            module_path = (dst_dir + file_name.replace(src_dir, "")) \
                .replace(".module.cpp", ".gmc")

            print(f"{module_name} {module_path}")
    else:
        log(f"File type not supported {file_name}")
