from cutekit import shell, vt100, cli
import subprocess
import re


def isVersionAtLeastOrEqual(
    actualVersion: tuple[int, ...], expectedVersion: tuple[int, ...]
) -> bool:
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


def checkVersion(
    cmd: str, versionExpected: tuple[int, ...], versionCommand: str, versionRegex: str
) -> tuple[bool, tuple[int, ...]]:
    try:
        result = subprocess.check_output([cmd, versionCommand]).decode("utf-8")
        actualVersion = getVersionFromOutput(result, versionRegex)
        return isVersionAtLeastOrEqual(actualVersion, versionExpected), actualVersion
    except Exception:
        return False, ()


def commandIsAvailable(
    cmd: str,
    versionExpected: tuple[int, ...] = (0, 0, 0),
    versionCommand: str = "--version",
    versionRegex: str = r"\d+(\.\d+)+",
) -> bool:
    print(f"Checking if {cmd} is available... ", end="")
    result = True

    try:
        cmd = shell.latest(cmd)
        path = shell.which(cmd) or cmd
        versionMatch, version = checkVersion(
            cmd, versionExpected, versionCommand, versionRegex
        )
        if not versionMatch:
            if versionExpected == (0, 0, 0):
                print(f"{vt100.RED}not found{vt100.RESET}")
            else:
                print(f"{vt100.RED}too old{vt100.RESET}")
                print(
                    f"Expected: {'.'.join(map(str, versionExpected))}\nActual: {'.'.join(map(str, version))}"
                )
            result = False
        else:
            print(f"{vt100.GREEN}ok{vt100.RESET}")
        print(
            f"{vt100.BRIGHT_BLACK}Command: {cmd}\nLocation: {path}\nVersion: {'.'.join(map(str, version))}{vt100.RESET}\n"
        )
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
            f"{vt100.BRIGHT_BLACK}Module: {module}\nVersion: {version}\nLocation: {path}{vt100.RESET}\n"
        )

        return True
    except Exception as e:
        print(f" {e}")
        print(f"{vt100.RED}Error: {module} is not available{vt100.RESET}")
        return False


def compileTest(what: str, code: str) -> bool:
    print(f"Checking if {what} is available...", end="")
    output = ""
    try:
        proc = subprocess.run(
            [
                shell.latest("clang++"),
                "-x",
                "c++",
                "-std=c++20",
                "-o",
                "/dev/null",
                "-",
            ],
            input=code.encode("utf-8"),
            stderr=subprocess.PIPE,
        )
        output = proc.stderr.decode("utf-8")
        if proc.returncode != 0:
            print(f" {vt100.RED}Failed{vt100.RESET}")
            print(f"{vt100.BRIGHT_BLACK}{output}{vt100.RESET}")
            return False
        else:
            print(f" {vt100.GREEN}ok{vt100.RESET}")
        return True
    except Exception:
        print(f" {vt100.RED}Failed{vt100.RESET}")
        return False


def checkForHeader(header: str) -> bool:
    return compileTest(f"header <{header}>", f"#include <{header}>\nint main() {{}}")


def checkForSymbol(header: str, symbol: str) -> bool:
    return compileTest(
        f"symbol {symbol}", f"#include <{header}>\nint main() {{ {symbol}; }}"
    )


@cli.command(None, "tools", "Manage the development tools")
def _():
    pass


@cli.command("n", "tools/nuke", "Nuke the development tools")
@cli.command("s", "tools/setup", "Setup the development environment")
def _():
    raise RuntimeError("Don't use ck directly, use ./skift.sh instead.")


@cli.command("d", "tools/doctor", "Check if all required commands are available")
def _():
    everythingIsOk = True

    everythingIsOk = everythingIsOk & moduleIsAvailable("requests")
    everythingIsOk = everythingIsOk & moduleIsAvailable("graphviz")
    everythingIsOk = everythingIsOk & moduleIsAvailable("magic")
    everythingIsOk = everythingIsOk & moduleIsAvailable("cutekit")
    everythingIsOk = everythingIsOk & moduleIsAvailable("chatty")
    everythingIsOk = everythingIsOk & commandIsAvailable("qemu-system-x86_64")
    everythingIsOk = everythingIsOk & commandIsAvailable("clang", versionExpected=(18,))
    everythingIsOk = everythingIsOk & commandIsAvailable(
        "clang++", versionExpected=(16,)
    )
    everythingIsOk = everythingIsOk & commandIsAvailable(
        "llvm-ar", versionExpected=(16,)
    )
    everythingIsOk = everythingIsOk & commandIsAvailable(
        "ld.lld", versionExpected=(16,)
    )
    everythingIsOk = everythingIsOk & commandIsAvailable("nasm")
    everythingIsOk = everythingIsOk & commandIsAvailable("ninja")
    everythingIsOk = everythingIsOk & commandIsAvailable(
        "cutekit", versionCommand="version"
    )
    everythingIsOk = everythingIsOk & commandIsAvailable("chatty")
    everythingIsOk = everythingIsOk & commandIsAvailable("pkg-config")

    everythingIsOk = everythingIsOk & commandIsAvailable("sgdisk")
    everythingIsOk = everythingIsOk & commandIsAvailable("mformat")
    everythingIsOk = everythingIsOk & commandIsAvailable("mcopy")
    everythingIsOk = everythingIsOk & commandIsAvailable("mmd")

    HEADERS = {
        "coroutine": [
            "std::coroutine_handle",
        ],
        "compare": [
            "std::strong_ordering",
            "std::weak_ordering",
            "std::partial_ordering",
        ],
        "initializer_list": [
            "std::initializer_list",
        ],
        "new": [],
        "utility": [],
        "memory": [],
        "limits.h": [
            "CHAR_BIT",
        ],
        "math.h": [],
        "cstddef": [
            "size_t",
            "ptrdiff_t",
            "std::nullptr_t",
        ],
        "stdint.h": [
            "uint8_t",
            "uint16_t",
            "uint32_t",
            "uint64_t",
            "int8_t",
            "int16_t",
            "int32_t",
            "int64_t",
        ],
        "string.h": [
            "memcpy",
            "memmove",
            "memset",
            "memcmp",
        ],
    }

    headersOk = True

    for header in HEADERS:
        headersOk = headersOk & checkForHeader(header)

        if not headersOk:
            continue

        for symbol in HEADERS[header]:
            headersOk = headersOk & checkForSymbol(header, symbol)

    if not headersOk:
        print(
            f"\n{vt100.RED}Error: Some headers are missing, please install the development packages for your distribution.{vt100.RESET}"
        )
        everythingIsOk = False

    else:
        print(f"\n{vt100.GREEN}Everythings looking good ​😉​👌​{vt100.RESET}\n")
