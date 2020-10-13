#!/usr/bin/python3

import sys
import os
import pprint

from utils.Lexer import Lexer
from utils.Generator import Generator
from ipc import Parser, EmitProtocol, EmitPeer, EmitConnection

pp = pprint.PrettyPrinter(indent=4)


def read_file_to_string(path: str) -> str:
    with open(path, 'r') as file:
        return file.read()

    return ""


lexer = Lexer(read_file_to_string(sys.argv[2]))
prot = Parser.protocol(lexer)

# pp.pprint(prot)

gen = Generator()

if sys.argv[1] == "--protocol":
    EmitProtocol.protocol(gen, prot["properties"]["name"], prot)
elif sys.argv[1] == "--server" and "server" in prot:
    EmitPeer.peer(gen, prot, prot["server"])
elif sys.argv[1] == "--client" and "client" in prot:
    EmitPeer.peer(gen, prot, prot["client"])
elif sys.argv[1] == "--client-connection" and "client" in prot:
    EmitConnection.connection(gen, prot, prot["client"])
elif sys.argv[1] == "--server-connection" and "client" in prot:
    EmitConnection.connection(gen, prot, prot["server"])

print(gen.finalize())
