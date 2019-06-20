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
    glyph = data["characters"][chr]
    outfp.write(struct.pack("IIIIIIII", ord(chr), glyph["x"], glyph["y"], glyph["width"], glyph["height"], glyph["originX"], glyph["originY"], glyph["advance"]))

outfp.write(struct.pack("IIIIIIII", 0, 0, 0, 0, 0, 0, 0, 0))

outfp.close()