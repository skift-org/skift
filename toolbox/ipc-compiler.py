#!/usr/bin/python3

import sys
import os
import pprint
pp = pprint.PrettyPrinter(indent=4)

def read_file_to_string(path : str) -> str:
    with open(path, 'r') as file:
        return file.read()

    return ""

def abort(message: str):
    raise Exception("ERROR: " + message)

class Lexer:
    def __init__(self, code : str):
        self.code = code
        self.offset = 0

    def ended(self):
        return self.offset >= len(self.code)

    def forward(self, n=1):
        for i in range(n):
            if not self.ended():
                self.offset = self.offset  + 1

    def peek(self, peek):
        offset = self.offset + peek

        if offset > len(self.code):
            return "\0"

        return self.code[offset]

    def current(self):
        return self.peek(0)

    def current_is(self, what):
        if self.ended():
            return False

        return self.code[self.offset] in what

    def current_is_word(self, what):
        for i in range(len(what)):
            if self.peek(i) != what[i]:
                return False

        return True

    def eat(self, what):
        while self.current_is(what) and not self.ended():
            self.forward()

    def skip_word(self, what: str) -> bool:
        if (self.current_is_word(what)):
            self.forward(len(what))
            return True

        return False

    def skip_and_expect(self, what: str):
        if not self.skip_word(what):
            abort("Expected " + what)

    def eat_whitespace(self):
        self.eat(" \t\n\r")


def identifier(lexer: Lexer) -> str:
    tmp = ""

    while lexer.current_is("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890"):
        tmp = tmp + lexer.current()
        lexer.forward()

    return tmp

def string(lexer: Lexer) -> str:
    lexer.skip_and_expect("\"")

    tmp = ""

    while not lexer.current_is("\""):
        tmp = tmp + lexer.current()
        lexer.forward()

    lexer.forward()

    return tmp

def property_(lexer: Lexer):
    lexer.eat_whitespace()
    property_id = identifier(lexer)
    lexer.eat_whitespace()
    lexer.skip_and_expect("=")
    lexer.eat_whitespace()
    property_value = string(lexer)

    return {property_id: property_value}

def enumeration(lexer: Lexer):
    lexer.eat_whitespace()
    enumeration_name = identifier(lexer)
    enumeration_values = []

    lexer.eat_whitespace()
    lexer.skip_and_expect("(")
    lexer.eat_whitespace()

    while not lexer.current_is(")"):
        enumeration_values.append(identifier(lexer))
        lexer.eat_whitespace()
        lexer.skip_and_expect(",")
        lexer.eat_whitespace()

    lexer.skip_and_expect(")")

    return {enumeration_name: enumeration_values}


def struct_body(lexer: Lexer):
    body = {}

    lexer.eat_whitespace()
    lexer.skip_and_expect("(")
    lexer.eat_whitespace()

    while not lexer.current_is(")"):
        field_type = identifier(lexer)
        lexer.eat_whitespace()
        field_name = identifier(lexer)
        lexer.eat_whitespace()
        lexer.skip_word(",")
        lexer.eat_whitespace()

        body.update({field_name: field_type})


    lexer.skip_and_expect(")")

    return body

def structure(lexer: Lexer):
    lexer.eat_whitespace()
    structure_name = identifier(lexer)
    structure_field = struct_body(lexer)

    return {structure_name: structure_field}

def request_or_event(lexer: Lexer):
    lexer.eat_whitespace()
    request_name = identifier(lexer)

    request_arguments = struct_body(lexer)

    lexer.eat_whitespace()

    request_response = {}
    if lexer.skip_word("->"):
        lexer.eat_whitespace()
        request_response = struct_body(lexer)

    return {request_name: {
        "arguments": request_arguments,
        "response": request_response
    }}


def protocol_field(lexer: Lexer, protocol):
    if lexer.skip_word("property"):
        protocol["properties"].update(property_(lexer))
    elif lexer.skip_word("struct"):
        protocol["structure"].update(structure(lexer))
    elif lexer.skip_word("enum"):
        protocol["enumerations"].update(enumeration(lexer))
    elif lexer.skip_word("request"):
        protocol["requests"].update(request_or_event(lexer))
    elif lexer.skip_word("event"):
        protocol["events"].update(request_or_event(lexer))

    lexer.eat_whitespace()


def protocol(lexer: Lexer):
    protocol = {
        "properties": {},
        "enumerations": {},
        "structure": {},
        "requests" : {},
        "events" : {},
    }

    lexer.eat_whitespace()

    lexer.skip_and_expect("protocol")

    lexer.eat_whitespace()
    lexer.skip_and_expect("(")
    lexer.eat_whitespace()

    while not lexer.current_is(")"):
        protocol_field(lexer, protocol)
        lexer.eat_whitespace()
        lexer.skip_and_expect(";")
        lexer.eat_whitespace()

    lexer.eat_whitespace()
    lexer.skip_and_expect(")")
    lexer.eat_whitespace()
    lexer.skip_and_expect(";")
    lexer.eat_whitespace()

    return protocol


lexer = Lexer(read_file_to_string(sys.argv[1]))

prot = protocol(lexer)

pp.pprint(prot)

def emit(*code):
    print(''.join(code))

def emit_struct(name, fields):
    emit("struct ", name)
    emit('{')
    for field in fields:
        emit('    ', fields[field], ' ', field, ';')
    emit('};')
    emit('')

emit("namespace protocol::", prot["properties"]["name"])
emit('{')
emit('')

emit('constexpr uint32_t MAGIC =', " 0x", prot["properties"]["id"], ";")
emit('')

emit('/* --- Enumerations ---------------------------------------------------------- */')
emit('')

enumeration = prot["enumerations"]

for enum in enumeration:
    emit("enum class ", enum)
    emit('{')
    for value in enumeration[enum]:
        emit('    ', value, ',')
    emit('}')
    emit('')


emit('/* --- Structure ------------------------------------------------------------- */')
emit('')

structure = prot["structure"]
for struct in structure:
    emit_struct(struct, structure[struct])

emit('/* --- Messages ------------------------------------------------------------- */')
emit('')

requests = prot["requests"]
events = prot["events"]
requests_and_events = dict(**requests, **events)

emit("enum MessageType")
emit('{')
for message in requests_and_events:
    emit('    ',message.upper(), ',')
emit('}')
emit('')

for message in requests_and_events:
    messagePascalCase = message.replace("_", " ").title().replace(" ", "") 

    emit_struct(messagePascalCase + 'Message', requests_and_events[message]["arguments"])

emit("struct Message")
emit('{')
emit('    ', 'uint32_t magic;')
emit('    ', 'MessageType type;')
emit('    ')
emit('    ','union ')
emit('    ','{')
for message in requests_and_events:
    messagePascalCase = message.replace("_", " ").title().replace(" ", "") 
    emit('    ','    ', messagePascalCase + 'Message' ,' ', message, ";")
emit('    ','};')
emit('}')
emit('')

emit('/* --- Response ------------------------------------------------------------- */')
emit('')

emit("enum ResponseType")
emit('{')
for message in requests:
    emit('    ',message.upper(), ',')
emit('}')
emit('')

for message in requests_and_events:
    messagePascalCase = message.replace("_", " ").title().replace(" ", "") 
    emit_struct(messagePascalCase + 'Response', requests_and_events[message]["response"])

emit("struct Response")
emit('{')
emit('    ', 'uint32_t magic;')
emit('    ', 'ResponseType type;')
emit('    ')
emit('    ','union ')
emit('    ','{')
for message in requests:
    messagePascalCase = message.replace("_", " ").title().replace(" ", "") 
    emit('    ','    ', messagePascalCase + 'Response',' ', message, ";")
emit('    ','};')
emit('}')
emit('')

emit('}')
