
import copy
import os

import utils


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
    env["cflags"] += ["-fsanitize=address", "-fsanitize=undefined"]
    env["cxxflags"] += ["-fsanitize=address", "-fsanitize=undefined"]
    env["ldflags"] += ["-fsanitize=address", "-fsanitize=undefined"]
    return env


def enableColors(env: dict) -> dict:
    env = copy.deepcopy(env)
    if (env["toolchain"] == "clang"):
        env["cflags"] += ["-fcolor-diagnostics"]
        env["cxxflags"] += ["-fcolor-diagnostics"]
    elif (env["toolchain"] == "gcc"):
        env["cflags"] += ["-fdiagnostics-color=alaways"]
        env["cxxflags"] += ["-fdiagnostics-color=always"]

    return env


def available() -> list:
    return [file.removesuffix(".json") for file in os.listdir("sdk/toolchains") if file.endswith(".json")]


def load(env: str) -> dict:
    if not env in available():
        raise utils.CliException("Environment '%s' not available" % env)
    result = utils.loadJson(f"sdk/toolchains/{env}.json")

    for key in PASSED_TO_BUILD:
        if isinstance(result[key], bool):
            if result[key]:
                result["cflags"] += [f"-D__sdk_{key}__"]
                result["cxxflags"] += [f"-D__sdk_{key}__"]
        else:
            result["cflags"] += [f"-D__sdk_{key}_{result[key]}__"]
            result["cxxflags"] += [f"-D__sdk_{key}_{result[key]}__"]

    result["cflags"] += ["-std=gnu2x",
                         "-Isrc", "-Wall", "-Wextra", "-Werror"]
    result["cxxflags"] += ["-std=gnu++2b", "-Isrc", "-Wall",
                           "-Wextra", "-Werror", "-fno-exceptions", "-fno-rtti"]

    result["hash"] = utils.objSha256(result, PASSED_TO_BUILD)
    result["key"] = utils.objKey(result, PASSED_TO_BUILD)
    result["dir"] = f".build/{result['hash'][:8]}"
    result["bindir"] = f"{result['dir']}/bin"
    result["objdir"] = f"{result['dir']}/obj"
    result["ninjafile"] = result["dir"] + "/build.ninja"

    result = enableColors(result)

    return result
