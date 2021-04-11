#!/usr/bin/python3

import json
import sys
import struct

in_filename = sys.argv[1]
out_filename = sys.argv[2]

infp = open(in_filename, 'r')
data = json.load(infp)
infp.close()

outfp = open(out_filename, 'wb')

for character in data["characters"]:
    glyph = data["characters"][character]
    codepoint = ord(character)

    outfp.write(struct.pack("I", codepoint))
    outfp.write(struct.pack("I", glyph["x"]))
    outfp.write(struct.pack("I", glyph["y"]))
    outfp.write(struct.pack("I", glyph["width"]))
    outfp.write(struct.pack("I", glyph["height"]))
    outfp.write(struct.pack("i", glyph["originX"]))
    outfp.write(struct.pack("i", glyph["originY"]))
    outfp.write(struct.pack("I", glyph["advance"]))

outfp.write(struct.pack("IIIIIiiI", 0, 0, 0, 0, 0, 0, 0, 0))

outfp.close()
