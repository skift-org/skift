import random


def idCmd(opts: dict, args: list[str]) -> None:
    u = hex(random.randint(0, 2**64))
    l = hex(random.randint(0, 2**64))
    print("128bits : " + u + l[2:])
    print("64bits  : " + l)
    print("32bits  : " + l[:10])
    print("16bits  : " + l[:6])
    print("8bits   : " + l[:4])


__plugin__ = {
    "name": "id",
    "desc": "Generate a 64bit random id",
    "func": idCmd,
}
