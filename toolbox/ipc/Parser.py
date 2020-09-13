from utils import Lexer


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


def request_or_signal(lexer: Lexer):
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


def peer_field(lexer: Lexer, peer):
    if lexer.skip_word("property"):
        peer["properties"].update(property_(lexer))
    elif lexer.skip_word("request"):
        peer["requests"].update(request_or_signal(lexer))
    elif lexer.skip_word("signal"):
        peer["signals"].update(request_or_signal(lexer))

    lexer.eat_whitespace()


def peer(lexer: Lexer, type_: str):
    peer = {
        "name": {},
        "type": type_,
        "properties": {},
        "requests": {},
        "signals": {},
    }

    lexer.eat_whitespace()
    peer["name"] = identifier(lexer)
    lexer.eat_whitespace()

    if not lexer.current_is("("):
        return peer

    lexer.skip_and_expect("(")
    lexer.eat_whitespace()

    while not lexer.current_is(")"):
        peer_field(lexer, peer)
        lexer.eat_whitespace()
        lexer.skip_and_expect(";")
        lexer.eat_whitespace()

    lexer.skip_and_expect(")")
    lexer.eat_whitespace()

    return peer


def protocol_field(lexer: Lexer, protocol):
    if lexer.skip_word("property"):
        protocol["properties"].update(property_(lexer))
    elif lexer.skip_word("struct"):
        protocol["structures"].update(structure(lexer))
    elif lexer.skip_word("enum"):
        protocol["enumerations"].update(enumeration(lexer))
    elif lexer.skip_word("client"):
        protocol["client"] = peer(lexer, "client")
    elif lexer.skip_word("server"):
        protocol["server"] = peer(lexer, "server")

    lexer.eat_whitespace()


def protocol(lexer: Lexer):
    protocol = {
        "properties": {},
        "enumerations": {},
        "structures": {},
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
