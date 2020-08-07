#!/bin/env python3
import sys
import os

def usage():
    print(f"Usage: {sys.argv[0]} <name> <name> ...")

if len(sys.argv) < 2:
    usage()

sysroot = os.env["SKIFT_SYSROOT"]
skift = os.path.join(sysroot, "..", "..", "..")
contrib = os.path.join(skift, "contrib")

for module in sys.argv[1:]:
    path = os.path.join(contrib, module)
    if not os.path.isdir(path):
        print(f"{} not found")
        continue
    os.chdir(path)
    if not os.system("./get-it.sh") == 0:
        print(f"Error while downloading {module}")
        continue
    if not os.system("./build-it.sh") == 0:
        print(f"Error while building {module}")
        continue
    if not os.system("./install-it.sh") == 0:
        print(f"Error while installing {module}")
        continue
    os.chdir(skift)
