from cutekit.cmds import Cmd, append
from cutekit.args import Args
from cutekit import shell, vt100
import subprocess
import re


def isVersionAtLeastOrEqual(actualVersion: tuple[int, ...], expectedVersion: tuple[int, ...]) -> bool:
    if len(actualVersion) < len(expectedVersion):
        return False

    for i in range(len(expectedVersion)):
        if actualVersion[i] < expectedVersion[i]:
            return False

    return True


def getVersionFromOutput(output: str, versionRegex: str) -> tuple[int, ...]:
    versionMatch = re.search(versionRegex, output)
    if versionMatch:
        versionStr = versionMatch[0].split(".")
        return tuple(map(int, versionStr))
    else:
        return ()


def checkVersion(cmd: str, versionExpected: tuple[int, ...], versionCommand: str, versionRegex: str) -> tuple[bool, tuple[int, ...]]:
    try:
        result = subprocess.check_output([cmd, versionCommand]).decode("utf-8")
        actualVersion = getVersionFromOutput(result, versionRegex)
        return isVersionAtLeastOrEqual(actualVersion, versionExpected), actualVersion
    except Exception as e:
        return False, ()


def commandIsAvailable(cmd: str, versionExpected: tuple[int, ...] = (0, 0, 0), versionCommand: str = "--version", versionRegex: str = r"\d+(\.\d+)+") -> bool:
    print(f"Checking if {cmd} is available... ", end="")
    result = True

    try:
        cmd = shell.latest(cmd)
        path = shell.which(cmd) or cmd
        versionMatch, version = checkVersion(
            cmd, versionExpected, versionCommand, versionRegex)
        if not versionMatch:
            if versionExpected == (0, 0, 0):
                print(f"{vt100.RED}not found{vt100.RESET}")
            else:
                print(f"{vt100.RED}too old{vt100.RESET}")
                print(
                    f"Expected: {'.'.join(map(str, versionExpected))}\nActual: {'.'.join(map(str, version))}")
            result = False
        else:
            print(f"{vt100.GREEN}ok{vt100.RESET}")
        print(
            f"{vt100.BRIGHT_BLACK}Command: {cmd}\nLocation: {path}\nVersion: {'.'.join(map(str, version))}{vt100.RESET}\n")
    except Exception as e:
        print(f" {e}")
        print(f"{vt100.RED}Error: {cmd} is not available{vt100.RESET}")
        result = False

    return result


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
    everythingIsOk = everythingIsOk and commandIsAvailable(
        "clang", versionExpected=(15,))
    everythingIsOk = everythingIsOk and commandIsAvailable(
        "clang++", versionExpected=(15,))
    everythingIsOk = everythingIsOk and commandIsAvailable(
        "ld.lld", versionExpected=(15,))
    everythingIsOk = everythingIsOk and commandIsAvailable("nasm")
    everythingIsOk = everythingIsOk and commandIsAvailable("ninja")
    everythingIsOk = everythingIsOk and commandIsAvailable(
        "cutekit", versionCommand="version")
    everythingIsOk = everythingIsOk and commandIsAvailable("chatty")
    everythingIsOk = everythingIsOk and commandIsAvailable("pkg-config")


append(Cmd(None, "doctor", "Check if all required commands are available", doctorCmd))
