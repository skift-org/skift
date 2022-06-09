
import copy
import os

import utils


ENVIRONMENTS = {}

ENVIRONMENTS["clang"] = {
    "id": "clang",
    "toolchain": "clang",

    "arch": "x86",
    "sub": "64",
    "vendor": "unknown",
    "sys": os.uname()[0].lower(),
    "abi": "unknown",
    "freestanding": False,

    "cc": "clang",
    "cflags": "",
    "cxx": "clang++",
    "cxxflags": "",
    "ld": "clang++",
    "ldflags": "",
    "ar": "llvm-ar",
    "arflags": "rcs",
    "as": "nasm",
    "asflags": "",
}

ENVIRONMENTS["gcc"] = {
    "id": "gcc",
    "toolchain": "gcc",

    "arch": "x86",
    "sub": "64",
    "vendor": "unknown",
    "sys": os.uname()[0].lower(),
    "abi": "unknown",
    "freestanding": False,

    "cc": "gcc",
    "cflags": "",
    "cxx": "g++",
    "cxxflags": "",
    "ld": "g++",
    "ldflags": "",
    "ar": "ar",
    "arflags": "rcs",
    "as": "nasm",
    "asflags": "",
}

ENVIRONMENTS["host"] = ENVIRONMENTS["clang"]

ENVIRONMENTS["efi-x86_64"] = {
    "id": "efi-x86_64",
    "toolchain": "clang",

    "arch": "x86",
    "sub": "64",
    "vendor": "unknown",
    "sys": "efi",
    "abi": "ms",
    "freestanding": True,

    "cc": "clang",
    "cflags": "-g -target x86_64-unknown-windows -ffreestanding -fno-stack-protector -fshort-wchar -mno-red-zone",
    "cxx": "clang++",
    "cxxflags": "-g -target x86_64-unknown-windows -ffreestanding -fno-stack-protector -fshort-wchar -mno-red-zone",
    "ld": "clang++",
    "ldflags": "-target x86_64-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link",
    "ar": "llvm-ar",
    "arflags": "rcs",
    "as": "nasm",
    "asflags": "",
}

ENVIRONMENTS["hjert-x86_32"] = {
    "id": "hjert-x86_32",
    "toolchain": "clang",

    "arch": "x86",
    "sub": "32",
    "vendor": "unknown",
    "sys": "hjert",
    "abi": "sysv",
    "freestanding": True,

    "cc": "clang -target i386-none-elf",
    "cflags": "-ffreestanding -fno-stack-protector",

    "cxx": "clang++ -target i386-none-elf",
    "cxxflags": "-ffreestanding -fno-stack-protector",

    "ld": "clang++ -target i386-none-elf",
    "ldflags": "-nostdlib",

    "ar": "llvm-ar",
    "arflags": "rcs",
    "as": "nasm",
    "asflags": "-f elf32 ",
}

ENVIRONMENTS["hjert-x86_64"] = {
    "id": "hjert-x86_64",
    "toolchain": "clang",

    "arch": "x86",
    "sub": "64",
    "vendor": "unknown",
    "sys": "hjert",
    "abi": "sysv",
    "freestanding": True,

    "cc": "clang -target x86_64-none-elf",
    "cflags": "-ffreestanding -fno-stack-protector -fno-stack-protector -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel",

    "cxx": "clang++ -target x86_64-none-elf",
    "cxxflags": "-ffreestanding -fno-stack-protector -fno-stack-protector -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel",

    "ld": "ld.lld",
    "ldflags": "-Tmeta/toolchains/hjert-x86_64.ld -z max-page-size=0x1000",

    "ar": "llvm-ar",
    "arflags": "rcs",
    "as": "nasm",
    "asflags": "-f elf64 ",
}

PASSED_TO_BUILD = [
    "toolchain", "arch", "sub", "vendor", "sys", "abi", "freestanding"]


def enableCache(env: dict) -> dict:
    env = copy.deepcopy(env)
    env["cc"] = "ccache " + env["cc"]
    env["cxx"] = "ccache " + env["cxx"]
    return env


def enableSan(env: dict) -> dict:
    if (env["freestanding"]):
        return env
    env = copy.deepcopy(env)
    env["cflags"] += " -fsanitize=address -fsanitize=undefined"
    env["cxxflags"] += " -fsanitize=address -fsanitize=undefined"
    env["ldflags"] += " -fsanitize=address -fsanitize=undefined"
    return env


def enableColors(env: dict) -> dict:
    env = copy.deepcopy(env)
    if (env["toolchain"] == "clang"):
        env["cflags"] += " -fcolor-diagnostics"
        env["cxxflags"] += " -fcolor-diagnostics"
    elif (env["toolchain"] == "gcc"):
        env["cflags"] += " -fdiagnostics-color=alaways"
        env["cxxflags"] += " -fdiagnostics-color=always"

    return env


def available() -> list:
    return list(ENVIRONMENTS.keys())


def load(env: str) -> dict:
    if not env in available():
        raise utils.CliException("Environment '%s' not available" % env)

    result = copy.deepcopy(ENVIRONMENTS[env])

    for key in PASSED_TO_BUILD:
        if isinstance(result[key], bool):
            if result[key]:
                result["cflags"] += f" -D__meta_{key}__"
                result["cxxflags"] += f" -D__meta_{key}__"
        else:
            result["cflags"] += f" -D__meta_{key}_{result[key]}__"
            result["cxxflags"] += f" -D__meta_{key}_{result[key]}__"

    result["cflags"] = "-std=gnu2x -Isrc -Wall -Wextra -Werror " + result["cflags"]
    result["cxxflags"] = "-std=gnu++2b -Isrc -Wall -Wextra -Werror -fno-exceptions -fno-rtti " + \
        result["cxxflags"]

    result["hash"] = utils.objSha256(result, PASSED_TO_BUILD)
    result["key"] = utils.objKey(result, PASSED_TO_BUILD)
    result["dir"] = f".build/{result['hash'][:8]}"
    result["bindir"] = f"{result['dir']}/bin"
    result["objdir"] = f"{result['dir']}/obj"
    result["ninjafile"] = result["dir"] + "/build.ninja"

    result = enableColors(result)

    return result
