from typing import Any, TextIO

import json
import os
import copy
import ninja
import sys


# --- Manifest Loading ------------------------------------------------------- #


def load_json(filename: str) -> Any:
    try:
        with open(filename) as f:
            data = json.load(f)
            data["dir"] = os.path.dirname(filename)
            return data
    except:
        print("Error loading manifest: " + filename)
        sys.exit(1)


def load_manifests(manifests: list) -> list:
    return [load_json(x) for x in manifests]


def find_manifests(dirname: str) -> list[str]:
    result = []
    for root, dirs, files in os.walk(dirname):
        for filename in files:
            if filename == 'manifest.json':
                result.append(os.path.join(root, filename))
    return result


def filter_manifests(manifests: list, env: dict) -> list:
    result = []
    for manifest in manifests:
        accepted = True

        if "requires" in manifest:
            for req in manifest["requires"]:
                if not env[req] in manifest["requires"][req]:
                    accepted = False
                    break

        if accepted:
            result.append(manifest)

    return result


def list2dict(list: list) -> dict:
    result = {}
    for item in list:
        result[item["id"]] = item
    return result


def inject_manifest(manifest: dict) -> dict:
    manifest = copy.deepcopy(manifest)
    for key in manifest:
        item = manifest[key]
        if "inject" in item:
            for inject in item["inject"]:
                if inject in manifest:
                    manifest[inject]["deps"].append(key)
    return manifest


def resolve_deps(manifest: dict) -> dict:
    manifest = copy.deepcopy(manifest)

    def resolve(key: str, stack: list[str] = []) -> list[str]:
        result: list[str] = []
        if key in stack:
            raise Exception("Circular dependency detected: " +
                            str(stack) + " -> " + key)

        stack.append(key)
        if "deps" in manifest[key]:
            result.extend(manifest[key]["deps"])
            for dep in manifest[key]["deps"]:
                result.extend(resolve(dep, stack))
        stack.pop()
        return result

    def strip_dups(l: list[str]) -> list[str]:
        # Remove duplicates from a list
        # by keeping only the last occurence
        result: list[str] = []
        for item in l:
            if item in result:
                result.remove(item)
            result.append(item)
        return result

    for key in manifest:
        manifest[key]["deps"] = strip_dups(resolve(key))

    return manifest


def find_source_files(manifest: dict) -> list:
    result = os.listdir(manifest["dir"])
    return [manifest["dir"] + "/" + x for x in result if x.endswith(".cpp") or x.endswith(".c")]


def find_tests_files(manifest: dict) -> list:
    if not os.path.isdir(manifest["dir"] + "/tests"):
        return []
    result = os.listdir(manifest["dir"] + "/tests")
    return [x for x in result if x.endswith(".cpp")]


def find_assets_files(manifest: dict) -> list:
    if not os.path.isdir(manifest["dir"] + "/assets"):
        return []
    return os.listdir(manifest["dir"] + "/assets")


def find_files(manifests: dict) -> dict:
    manifest = copy.deepcopy(manifests)
    for key in manifest:
        item = manifest[key]
        item["tests"] = find_tests_files(item)
        item["assets"] = find_assets_files(item)
        item["srcs"] = find_source_files(item)
        item["objs"] = [(x.replace("src/", "obj/") + ".o", x)
                        for x in item["srcs"]]

        if item["type"] == "lib":
            item["out"] = "bin/" + key + ".a"
        elif item["type"] == "exe":
            item["out"] = "bin/" + key + ".elf"
        else:
            raise Exception("Unknown type: " + item["type"])

    return manifest


def find_libs(manifests: dict) -> dict:
    manifest = copy.deepcopy(manifests)
    for key in manifest:
        item = manifest[key]
        item["libs"] = [manifest[x]["out"]
                        for x in item["deps"] if manifest[x]["type"] == "lib"]

    return manifest


# --- Evironment ------------------------------------------------------------- #

ENV_HOST = {
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
    "arflags": "",
    "as": "nasm",
    "asflags": "",
}

ENV_EFI = {
    "toolchain": "clang",

    "arch": "x86",
    "sub": "64",
    "vendor": "unknown",
    "sys": "efi",
    "abi": "ms",
    "freestanding": True,

    "cc": "clang",
    "cflags": "-target x86_64-unknown-windows -ffreestanding -fno-stack-protector -fshort-wchar -mno-red-zone",
    "cxx": "clang++",
    "cxxflags": "-target x86_64-unknown-windows -ffreestanding -fno-stack-protector -fshort-wchar -mno-red-zone",
    "ld": "clang++",
    "ldflags": "-target x86_64-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link",
    "ar": "llvm-ar",
    "arflags": "",
    "as": "nasm",
    "asflags": "",
}

PASS = ["toolchain", "arch", "sub", "vendor", "sys", "abi", "freestanding"]


def preprocess_env(env: dict, manifests: dict) -> dict:
    result = copy.deepcopy(env)

    include_paths = []

    for key in manifests:
        item = manifests[key]
        if "root-include" in item:
            include_paths.append(item["dir"])

    if len(include_paths) > 0:
        result["cflags"] += " -I" + " -I".join(include_paths)
        result["cxxflags"] += " -I" + " -I".join(include_paths)

    for key in PASS:
        if isinstance(result[key], bool):
            if result[key]:
                result["cflags"] += f" -D__sk_{key}__"
                result["cxxflags"] += f" -D__sk_{key}__"
        else:
            result["cflags"] += f" -D__sk_{key}_{result[key]}__"
            result["cxxflags"] += f" -D__sk_{key}_{result[key]}__"

    result["cflags"] = "-std=gnu2x -Isrc -Wall -Wextra -Werror -fcolor-diagnostics " + result["cflags"]
    result["cxxflags"] = "-std=gnu++2b -Isrc -Wall -Wextra -Werror -fcolor-diagnostics -fno-exceptions -fno-rtti " + \
        result["cxxflags"]

    return result

# --- Generator -------------------------------------------------------------- #


def generate_ninja(out: TextIO, manifests: dict, env: dict) -> None:
    writer = ninja.Writer(out)

    writer.comment("Generated by sk.py")
    writer.newline()

    writer.comment("Environment:")
    for key in env:
        writer.variable(key, env[key])
    writer.newline()

    writer.comment("Rules:")
    writer.rule(
        "cc", "$cc -c -o $out $in -MD -MF $out.d $cflags", depfile="$out.d")
    writer.rule(
        "cxx", "$cxx -c -o $out $in -MD -MF $out.d $cxxflags", depfile="$out.d")
    writer.rule("ld", "$ld -o $out $in $ldflags")
    writer.rule("ar", "$ar crs $out $in")
    writer.rule("as", "$as -o $out $in $asflags")
    writer.newline()

    writer.comment("Build:")
    all = []
    for key in manifests:
        item = manifests[key]

        writer.comment("Project: " + item["id"])

        for obj in item["objs"]:
            if obj[1].endswith(".c"):
                writer.build(obj[0], "cc", obj[1])
            elif obj[1].endswith(".cpp"):
                writer.build(obj[0], "cxx", obj[1])

        writer.newline()

        objs = [x[0] for x in item["objs"]]

        if item["type"] == "lib":
            writer.build(item["out"], "ar", objs)
        else:
            objs = objs + item["libs"]
            writer.build(item["out"], "ld", objs)

        all.append(item["out"])

        writer.newline()

    writer.comment("Phony:")
    writer.build("all", "phony", all)


environment = ENV_EFI

manifests_list = load_manifests(find_manifests("."))
manifests = list2dict(filter_manifests(manifests_list, environment))
manifests = inject_manifest(manifests)
manifests = resolve_deps(manifests)
manifests = find_files(manifests)
manifests = find_libs(manifests)

environment = preprocess_env(environment, manifests)
print(json.dumps(environment, indent=4))
generate_ninja(open("build.ninja", "w"), manifests, environment)
