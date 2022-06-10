import shutil
import sys

import build
import utils
from utils import Colors
import environments


CMDS = {}


def parseOptions(args: list[str]) -> dict:
    result = {
        'opts': {},
        'args': []
    }

    for arg in args:
        if arg.startswith("--"):
            if "=" in arg:
                key, value = arg[2:].split("=", 1)
                result['opts'][key] = value
            else:
                result['opts'][arg[2:]] = True
        else:
            result['args'].append(arg)

    return result


def runCmd(opts: dict, args: list[str]) -> None:
    if len(args) == 0:
        print(f"Usage: {sys.argv[0]} run <component>")
        sys.exit(1)

    out = build.buildOne(opts.get('env', 'host'), args[0])
    utils.runCmd(out, *args[1:])


def bootCmd(opts: dict, args: list[str]) -> None:
    imageDir = utils.mkdirP(".build/image")
    bootDir = utils.mkdirP(".build/image/EFI/BOOT")

    hjert = build.buildOne("hjert-x86_64", "hjert")
    loader = build.buildOne("efi-x86_64", "loader")
    shutil.copy(hjert, f"{bootDir}/hjert.elf")
    shutil.copy(loader, f"{bootDir}/BOOTX64.EFI")

    ovmf = utils.downloadFile(
        "https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd")

    utils.runCmd(
        "qemu-system-x86_64",
        "-no-reboot",
        "-no-shutdown",
        "-d", "cpu_reset,guest_errors",
        "-serial", "mon:stdio",
        "-bios", ovmf,
        "-m", "512",
        "-smp", "4",
        "-drive", f"file=fat:rw:{imageDir},media=disk,format=raw")


def buildCmd(opts: dict, args: list[str]) -> None:
    env = opts.get('env', 'host')
    if len(args) == 0:
        build.buildAll(env)
    else:
        print("Building:")
        for component in args:
            print(f"  {component}")
            build.buildOne(env, component)


def cleanCmd(opts: dict, args: list[str]) -> None:
    shutil.rmtree(".build", ignore_errors=True)


def nukeCmd(opts: dict, args: list[str]) -> None:
    shutil.rmtree(".build", ignore_errors=True)
    shutil.rmtree(".cache", ignore_errors=True)


def idCmd(opts: dict, args: list[str]) -> None:
    import random
    print(hex(random.randint(0, 2**64)))


def helpCmd(opts: dict, args: list[str]) -> None:
    print(f"Usage: {sys.argv[0]} <command> [options...] [<args...>]")
    print("")

    print("Description:")
    print("   The skift operating system build system.")
    print("")

    print("Commands:")
    for cmd in CMDS:
        print("  " + cmd + " - " + CMDS[cmd]["desc"])
    print("")

    print("Enviroments:")
    for env in environments.available():
        print("  " + env)
    print("")


CMDS = {
    "run": {
        "func": runCmd,
        "desc": "Run a component on the host",
    },
    "boot": {
        "func": bootCmd,
        "desc": "Boot a component in a QEMU instance",
    },
    "build": {
        "func": buildCmd,
        "desc": "Build one or more components",
    },
    "clean": {
        "func": cleanCmd,
        "desc": "Clean the build directory",
    },
    "nuke": {
        "func": nukeCmd,
        "desc": "Clean the build directory and cache",
    },
    "id": {
        "func": idCmd,
        "desc": "Generate a 64bit random id",
    },
    "help": {
        "func": helpCmd,
        "desc": "Show this help message",
    },
}

if __name__ == "__main__":
    try:
        if len(sys.argv) < 2:
            helpCmd({}, [])
        else:
            o = parseOptions(sys.argv[2:])
            if not sys.argv[1] in CMDS:
                print(f"Unknown command: {sys.argv[1]}")
                print("")
                print(f"Use '{sys.argv[0]} help' for a list of commands")
                sys.exit(1)
            CMDS[sys.argv[1]]["func"](o['opts'], o['args'])
            sys.exit(0)
    except utils.CliException as e:
        print()
        print(f"{Colors.RED}{e.msg}{Colors.RESET}")
        sys.exit(1)
