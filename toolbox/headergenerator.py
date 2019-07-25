#!/bin/python3

import sys
import struct

in_filename = sys.argv[1]
out_filename = sys.argv[2]

infp = open(in_filename, 'r')  
data = json.load(infp)
infp.close()

outfp = open(out_filename, 'wb')

# TODO: compile the json keymap to a keymap file loadable by the kernel

outfp.close()