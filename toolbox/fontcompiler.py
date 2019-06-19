#!/bin/python3

import json
import sys
import struct

in_filename = sys.argv[1]
out_filename = sys.argv[2]

infp = open(in_filename, 'r')  
data = json.load(infp)
infp.close()

outfp = open(out_filename, 'wb')

for chr in data["characters"]:
    glypth = data["characters"][chr]
    outfp.write(struct.pack("I", ord(chr)))
    outfp.write(struct.pack("IIIIIIII", ord(chr), glypth["x"], glypth["y"], glypth["width"], glypth["height"], glypth["originX"], glypth["originY"], glypth["advance"]))

outfp.write(struct.pack("IIIIIIII", 0, 0, 0, 0, 0, 0, 0, 0))

outfp.close()