import os
import json
import copy

import utils


def loadJsons(basedir: str) -> dict:
    result = {}
    for root, dirs, files in os.walk(basedir):
        for filename in files:
            if filename == 'manifest.json':
                filename = os.path.join(root, filename)
                try:
                    with open(filename) as f:
                        manifest = json.load(f)
                        manifest["dir"] = os.path.dirname(filename)
                        result[manifest["id"]] = manifest
                except Exception as e:
                    raise utils.CliException(
                        f"Failed to load manifest {filename}: {e}")

    return result


def filter(manifests: dict, env: dict) -> dict:
    result = {}
    for id in manifests:
        manifest = manifests[id]
        accepted = True

        if "requires" in manifest:
            for req in manifest["requires"]:
                if not env[req] in manifest["requires"][req]:
                    accepted = False
                    break

        if accepted:
            result[id] = manifest

    return result


def doInjects(manifests: dict) -> dict:
    manifests = copy.deepcopy(manifests)
    for key in manifests:
        item = manifests[key]
        if "inject" in item:
            for inject in item["inject"]:
                if inject in manifests:
                    manifests[inject]["deps"].append(key)
    return manifests


def resolveDeps(manifests: dict) -> dict:
    manifests = copy.deepcopy(manifests)

    def resolve(key: str, stack: list[str] = []) -> list[str]:
        result: list[str] = []
        if key in stack:
            raise utils.CliException("Circular dependency detected: " +
                                     str(stack) + " -> " + key)

        if not key in manifests:
            raise utils.CliException("Unknown dependency: " + key)

        if "deps" in manifests[key]:
            stack.append(key)
            result.extend(manifests[key]["deps"])
            for dep in manifests[key]["deps"]:
                result += resolve(dep, stack)
            stack.pop()
        return result

    for key in manifests:
        manifests[key]["deps"] = utils.stripDups(resolve(key))

    return manifests


def findFiles(manifests: dict) -> dict:
    manifests = copy.deepcopy(manifests)

    for key in manifests:
        item = manifests[key]
        path = manifests[key]["dir"]
        testsPath = os.path.join(path, "tests")
        assetsPath = os.path.join(path, "assets")

        item["tests"] = utils.findFiles(testsPath, [".c", ".cpp"])
        item["srcs"] = utils.findFiles(path, [".c", ".cpp", ".s"])
        item["assets"] = utils.findFiles(assetsPath)

    return manifests


def prepareTests(manifests: dict) -> dict:
    if not "tests" in manifests:
        return manifests
    manifests = copy.deepcopy(manifests)
    tests = manifests["tests"]

    for key in manifests:
        item = manifests[key]
        if "tests" in item and len(item["tests"]) > 0:
            tests["deps"] += [item["id"]]
            tests["srcs"] += item["tests"]

    return manifests


def prepareInOut(manifests: dict, env: dict) -> dict:
    manifests = copy.deepcopy(manifests)
    for key in manifests:
        item = manifests[key]
        basedir = os.path.dirname(item["dir"])

        item["objs"] = [(x.replace(basedir, env["objdir"] + "/") + ".o", x)
                        for x in item["srcs"]]

        if item["type"] == "lib":
            item["out"] = env["bindir"] + "/" + key + ".a"
        elif item["type"] == "exe":
            item["out"] = env["bindir"] + "/" + key
        else:
            raise utils.CliException("Unknown type: " + item["type"])

    for key in manifests:
        item = manifests[key]
        item["libs"] = [manifests[x]["out"]
                        for x in item["deps"] if manifests[x]["type"] == "lib"]
    return manifests


def cincludes(manifests: dict) -> str:
    include_paths = []

    for key in manifests:
        item = manifests[key]
        if "root-include" in item:
            include_paths.append(item["dir"])

    if len(include_paths) == 0:
        return ""

    return " -I" + " -I".join(include_paths)


cache: dict = {}


def loadAll(basedir: str, env: dict) -> dict:
    cacheKey = basedir + ":" + env["id"]
    if cacheKey in cache:
        return cache[cacheKey]

    manifests = loadJsons(basedir)
    manifests = filter(manifests, env)
    manifests = doInjects(manifests)
    manifests = resolveDeps(manifests)
    manifests = findFiles(manifests)
    manifests = prepareTests(manifests)
    manifests = prepareInOut(manifests, env)

    cache[cacheKey] = manifests
    return manifests
