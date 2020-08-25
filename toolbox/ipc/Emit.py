from utils import Generator


def camelcase(name: str):
    return name.replace("_", " ").title().replace(" ", "")


def struct(gen: Generator, name: str, fields):
    gen.emit("struct ", name)
    gen.emit('{')
    gen.push_ident()

    for field in fields:
        gen.emit(fields[field], ' ', field, ';')

    gen.pop_ident()
    gen.emit('};')


def enumeration(gen: Generator, name: str, members):
    gen.emit("enum class ", name)
    gen.emit('{')
    gen.push_ident()

    for meb in members:
        gen.emit(meb, ',')

    gen.pop_ident()
    gen.emit('};')


def message_types(gen: Generator, peer):
    gen.emit("enum class MessageType")
    gen.emit('{')
    gen.push_ident()

    for meb in dict(**peer["requests"], **peer["events"]):
        gen.emit(meb.upper(), ',')

    gen.pop_ident()
    gen.emit('};')


def message_payloads(gen: Generator, peer):
    requests_and_events = dict(**peer["requests"], **peer["events"])

    for meb in requests_and_events:
        if len(requests_and_events[meb]["arguments"]) > 0:
            struct(gen, camelcase(meb) + "Message",
                   requests_and_events[meb]["arguments"])
            gen.emit('')


def message_any_payload(peer):
    requests_and_events = dict(**peer["requests"], **peer["events"])

    for meb in requests_and_events:
        if len(requests_and_events[meb]["arguments"]) > 0:
            return True

    return False


def message_packet(gen: Generator, peer):
    gen.emit("struct Message")
    gen.emit("{")
    gen.push_ident()

    gen.emit("uint32_t magic;")
    gen.emit("MessageType type;")

    if message_any_payload(peer):
        gen.emit("union {")
        gen.push_ident()

        requests_and_events = dict(**peer["requests"], **peer["events"])
        for meb in requests_and_events:
            if len(requests_and_events[meb]["arguments"]) > 0:
                gen.emit(camelcase(meb) + "Message", " ", meb)

        gen.pop_ident()
        gen.emit("};")

    gen.pop_ident()
    gen.emit("};")


def response_types(gen: Generator, peer):
    gen.emit("enum class ResponseType")
    gen.emit('{')
    gen.push_ident()

    for meb in peer["requests"]:
        gen.emit(meb.upper(), ',')

    gen.pop_ident()
    gen.emit('};')


def response_payload(gen: Generator, peer):
    for meb in peer["requests"]:
        if len(peer["requests"][meb]["response"]) > 0:
            struct(gen, camelcase(meb) + "Response",
                   peer["requests"][meb]["response"])
            gen.emit('')


def response_any_payload(peer):
    for meb in peer["requests"]:
        if len(peer["requests"][meb]["response"]) > 0:
            return True

    return False


def response_packed(gen: Generator, peer):
    gen.emit("struct Response")
    gen.emit("{")
    gen.push_ident()

    gen.emit("uint32_t magic;")
    gen.emit("ResponseType type;")
    
    if response_any_payload(peer):
        gen.emit("union {")
        gen.push_ident()

        requests_and_events = peer["requests"]
        for meb in requests_and_events:
            if len(peer["requests"][meb]["response"]) > 0:
                gen.emit(camelcase(meb) + "Response", " ", meb)

        gen.pop_ident()
        gen.emit("};")

    gen.pop_ident()
    gen.emit("};")


def peer(gen: Generator, name: str, peer):
    message_types(gen, peer)
    gen.emit('')

    message_payloads(gen, peer)

    message_packet(gen, peer)
    gen.emit('')

    if len(peer["requests"]) > 0:
        response_types(gen, peer)
        gen.emit('')

        response_payload(gen, peer)

        response_packed(gen, peer)
        gen.emit('')


def protocol(gen: Generator, name: str, protocol):
    gen.emit("#pragma once")  # u mad molly ?

    gen.emit("")

    gen.emit("namespace protocol::" + protocol["properties"]["name"])
    gen.emit("{")

    gen.push_ident()

    if len(protocol["enumerations"]) > 0:
        gen.emit_section("Enumerations")
        for e in protocol["enumerations"]:
            enumeration(gen, e, protocol["enumerations"][e])

    if len(protocol["structures"]) > 0:
        gen.emit_section("Structures")

        print(protocol["structures"])

        for s in protocol["structures"]:
            struct(gen, s, protocol["structures"][s])

    if len(protocol["peers"]) > 0:
        for p in protocol["peers"]:
            gen.emit_section(camelcase(p))
            peer(gen, p, protocol["peers"][p])

    gen.pop_ident()
    gen.emit("}")
