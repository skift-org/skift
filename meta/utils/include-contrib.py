#!/bin/env python3
import sys
import os

if not "SKIFT_SYSROOT" in os.environ:
    print("Please run use-it.sh first")
    exit(1)

def usage():
    print(f"Usage: {sys.argv[0]} <name> <name> ...")

if len(sys.argv) < 2:
    usage()

sysroot = os.environ["SKIFT_SYSROOT"]
contrib = os.environ["SKIFT_CONTRIBROOT"]
skift = os.path.join(contrib, "..")

for module in sys.argv[1:]:
    path = os.path.join(contrib, module)
    if not os.path.isdir(path):
        print(f"{path} not found")
        continue
    os.chdir(path)
    if not os.system("./clean-it.sh") == 0:
        print(f"Error while cleaning {module}")
        continue
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
os.chdir(contrib)
os.system("git checkout -- *")
