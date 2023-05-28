from cutekit.cmds import Cmd, append
from cutekit.args import Args
import random


def idCmd(args: Args):
    u = hex(random.randint(0, 2**64))
    l = hex(random.randint(0, 2**64))
    print("128bits : " + u + l[2:])
    print("64bits  : " + l)
    print("32bits  : " + l[:10])
    print("16bits  : " + l[:6])
    print("8bits   : " + l[:4])


append(Cmd(None, "id", "Generate a 64bit random id", idCmd))
