
import copy
import os

import utils


PASSED_TO_BUILD = [
    "toolchain", "arch", "sub", "vendor", "sys", "abi", "encoding", "freestanding", "variant"]


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


def enableOptimizer(env: dict, level: str) -> dict:
    env = copy.deepcopy(env)
    env["cflags"] += ["-O%s" % level]
    env["cxxflags"] += ["-O%s" % level]
    return env


def available() -> list:
    return [file.removesuffix(".json") for file in os.listdir("meta/toolchains") if file.endswith(".json")]


VARIANTS = ["debug", "devel", "release", "sanatize"]


def load(env: str) -> dict:
    variant = "devel"
    if ":" in env:
        env, variant = env.split(":")

    if not env in available():
        raise utils.CliException(f"Environment '{env}' not available")

    if not variant in VARIANTS:
        raise utils.CliException(f"Variant '{variant}' not available")

    result = utils.loadJson(f"meta/toolchains/{env}.json")
    result["variant"] = variant

    for key in PASSED_TO_BUILD:
        if isinstance(result[key], bool):
            if result[key]:
                result["cflags"] += [f"-D__sdk_{key}__"]
                result["cxxflags"] += [f"-D__sdk_{key}__"]
        else:
            result["cflags"] += [f"-D__sdk_{key}_{result[key]}__"]
            result["cxxflags"] += [f"-D__sdk_{key}_{result[key]}__"]

    result["cflags"] += [
        "-std=gnu2x",
        "-Isrc",
        "-Wall",
        "-Wextra",
        "-Werror"
    ]

    result["cxxflags"] += [
        "-std=gnu++2b",
        "-Isrc",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-fno-exceptions",
        "-fno-rtti"
    ]

    result["hash"] = utils.objSha256(result, PASSED_TO_BUILD)
    result["key"] = utils.objKey(result, PASSED_TO_BUILD)
    result["dir"] = f".build/{result['hash'][:8]}"
    result["bindir"] = f"{result['dir']}/bin"
    result["objdir"] = f"{result['dir']}/obj"
    result["ninjafile"] = result["dir"] + "/build.ninja"

    result = enableColors(result)

    if variant == "debug":
        result = enableOptimizer(result, "g")
    elif variant == "devel":
        result = enableOptimizer(result, "2")
    elif variant == "release":
        result = enableOptimizer(result, "3")
    elif variant == "sanatize":
        result = enableOptimizer(result, "g")
        result = enableSan(result)

    return result
