import json
import os
import re
import sys

with open(sys.argv[1], 'r') as f:
    data = json.load(f)

    for k in data:
        v = data[k]
        print(
            f"ICON({k.upper().replace('-', '_')}, \"{k}\", 0x{v.lower()})")
