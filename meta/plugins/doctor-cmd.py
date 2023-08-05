from cutekit.cmds import Cmd, append
from cutekit.args import Args
from cutekit import shell, vt100
import subprocess


def commandIsAvailable(cmd: str) -> bool:
    print(f"Checking if {cmd} is available...", end="")
    try:
        cmd = shell.latest(cmd)
        out = subprocess.check_output([cmd, "--version"])
        print(f"{vt100.GREEN} ok{vt100.RESET}")
        print(
            f"{vt100.BRIGHT_BLACK}Command: {cmd}\nVersion: {out.decode('utf-8').strip()}{vt100.RESET}")
        print()
        return True
    except Exception as e:
        print(f" {e}")
        print(f"{vt100.RED}Error: {cmd} is not available{vt100.RESET}")
        return False


def doctorCmd(args: Args):
    commandIsAvailable("qemu-system-x86_64")
    commandIsAvailable("clang")
    commandIsAvailable("clang++")
    commandIsAvailable("ld.lld")
    commandIsAvailable("nasm")
    commandIsAvailable("ninja")
    commandIsAvailable("chatty")


append(Cmd(None, "doctor", "Check if all required commands are available", doctorCmd))
