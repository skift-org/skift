#!/usr/bin/python3

import sys
import os
import pprint

from utils.Lexer import Lexer
from utils.Generator import Generator
from ipc import Parser
from ipc import Emit

pp = pprint.PrettyPrinter(indent=4)


def read_file_to_string(path: str) -> str:
    with open(path, 'r') as file:
        return file.read()

    return ""


lexer = Lexer(read_file_to_string(sys.argv[1]))
prot = Parser.protocol(lexer)

pp.pprint(prot)

gen = Generator()
Emit.protocol(gen, prot["properties"]["name"], prot)
print(gen.finalize())
