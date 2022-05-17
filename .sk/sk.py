#!/usr/bin/env python

import json
from typing import Callable
import cache
import argparse
import utils


def strips_nulls(d: list) -> list:
    return [x for x in d if x is not None]


def list_to_dict(l: list) -> dict:
    d = {}
    for i in l:
        d[i["id"]] = i
    return d


code, data = cache.make("dump")
pkgs_list = strips_nulls(json.loads(data))
pkgs = list_to_dict(pkgs_list)

Cmd = Callable[[list], int]


def list_cmd(args: list) -> int:
    for pkg in pkgs:
        print(pkg)
    return 0


def dump_cmd(args: list) -> int:
    if (len(args) == 0):
        print(json.dumps(pkgs_list, indent=4))
        return 0
    else:
        result = []
        for pkg in args:
            if not pkg in pkgs:
                print("Package not found: {}".format(pkg))
                return -1
            result.append(pkgs[pkg])
        print(json.dumps(result, indent=4))
        return 0


def build_cmd(args: list) -> int:
    if (len(args) == 0):
        print("No packages specified")
        return 1

    for pkg in args:
        if not pkg in pkgs:
            print("Package not found: {}".format(pkg))
            continue

        pkg = pkgs[pkg]
        code, _ = utils.make(pkg["bin"])
        if (code != 0):
            return code

    return 0


def run_cmd(args: list) -> int:
    if build_cmd(args) != 0:
        return 1

    if (len(args) == 0):
        print("No packages specified")
        return 1

    for pkg in args:
        if not pkg in pkgs:
            print("Package not found: {}".format(pkg))
            continue

        pkg = pkgs[pkg]
        code, _ = utils.popen([pkg["bin"]])
        if (code != 0):
            return code

    return 0


def nuke_cmd(args: list) -> int:
    cache.nuke()
    return 0


SUBCMDS: dict[str, Cmd] = {
    "list": list_cmd,
    "dump": dump_cmd,
    "build": build_cmd,
    "run": run_cmd,
    "nuke": nuke_cmd,
}

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("subcmd", choices=SUBCMDS.keys())
    parser.add_argument("args", nargs="*")
    args = parser.parse_args()
    exit(SUBCMDS[args.subcmd](args.args))
