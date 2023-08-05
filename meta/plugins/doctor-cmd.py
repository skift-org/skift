from cutekit.cmds import Cmd, append
from cutekit.args import Args
from cutekit import shell, vt100
import subprocess


def commandIsAvailable(cmd: str) -> bool:
    print(f"Checking if {cmd} is available...", end="")
    try:
        cmd = shell.latest(cmd)
        path = shell.which(cmd) or cmd
        out = subprocess.check_output(
            [cmd, "version" if cmd == "cutekit" else "--version"])
        print(f"{vt100.GREEN} ok{vt100.RESET}")
        print(
            f"{vt100.BRIGHT_BLACK}Command: {cmd}\nLocation: {path}\nVersion: {out.decode('utf-8').strip()}{vt100.RESET}\n")
        return True
    except Exception as e:
        print(f" {e}")
        print(f"{vt100.RED}Error: {cmd} is not available{vt100.RESET}")
        return False


def moduleIsAvailable(module: str) -> bool:
    print(f"Checking if {module} is available...", end="")
    try:
        mod = __import__(module)
        print(f"{vt100.GREEN} ok{vt100.RESET}")
        version = "unknown"
        path = mod.__file__

        if hasattr(mod, "__version__"):
            version = mod.__version__
        print(
            f"{vt100.BRIGHT_BLACK}Module: {module}\nVersion: {version}\nLocation: {path}{vt100.RESET}\n")

        return True
    except Exception as e:
        print(f" {e}")
        print(f"{vt100.RED}Error: {module} is not available{vt100.RESET}")
        return False


def doctorCmd(args: Args):
    everythingIsOk = True

    everythingIsOk = everythingIsOk and moduleIsAvailable("requests")
    everythingIsOk = everythingIsOk and moduleIsAvailable("graphviz")
    everythingIsOk = everythingIsOk and moduleIsAvailable("magic")
    everythingIsOk = everythingIsOk and moduleIsAvailable("cutekit")
    everythingIsOk = everythingIsOk and moduleIsAvailable("chatty")
    everythingIsOk = everythingIsOk and commandIsAvailable(
        "qemu-system-x86_64")
    everythingIsOk = everythingIsOk and commandIsAvailable("clang")
    everythingIsOk = everythingIsOk and commandIsAvailable("clang++")
    everythingIsOk = everythingIsOk and commandIsAvailable("ld.lld")
    everythingIsOk = everythingIsOk and commandIsAvailable("nasm")
    everythingIsOk = everythingIsOk and commandIsAvailable("ninja")
    everythingIsOk = everythingIsOk and commandIsAvailable("cutekit")
    everythingIsOk = everythingIsOk and commandIsAvailable("chatty")
    everythingIsOk = everythingIsOk and commandIsAvailable("pkg-config")

    if everythingIsOk:
        print(f"{vt100.GREEN}Everything is ok{vt100.RESET}")
    else:
        print(f"{vt100.RED}Some things are not ok{vt100.RESET}")


append(Cmd(None, "doctor", "Check if all required commands are available", doctorCmd))
