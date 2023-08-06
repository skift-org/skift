from cutekit.cmds import Cmd, append
from cutekit.args import Args
import random


def nop(args: Args):
    print("Don't use ck directly, use ./skift.sh instead.")


append(Cmd(None, "setup", "Setup the development environment", nop))
append(Cmd(None, "nuke-tools", "Nuke the development tools", nop))
