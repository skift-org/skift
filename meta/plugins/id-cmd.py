import random


def idCmd(opts: dict, args: list[str]) -> None:
    i = hex(random.randint(0, 2**64))
    print("64bit: " + i)
    print("32bit: " + i[:10])


__plugin__ = {
    "name": "id",
    "desc": "Generate a 64bit random id",
    "func": idCmd,
}
