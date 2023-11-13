import random

from cutekit import cli, ensure

ensure((0, 6, 0))


@cli.command(None, "id", "Generate a 64bit random id")
def idCmd(args: cli.Args):
    u = hex(random.randint(0, 2**64))
    l = hex(random.randint(0, 2**64))
    print("128bits : " + u + l[2:])
    print("64bits  : " + l)
    print("32bits  : " + l[:10])
    print("16bits  : " + l[:6])
    print("8bits   : " + l[:4])
