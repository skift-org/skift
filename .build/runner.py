#!/usr/bin/env python

import json
import utils
import cache
import argparse
from typing import Optional

code, data = cache.make("dump")
pkgs = json.loads(data.replace("}, ]", "}]"))


def build_command(parent: argparse.ArgumentParser) -> None:
    parser = parent.add_subparsers(title="build", description="build commands")


def cache_command(parent: argparse.ArgumentParser) -> None:
    parser = parent.add_subparsers(
        title="cache", description="Manage the cache")
    parser.add_parser("nuke", help="Nuke the cache")
    parser.add_parser("dump", help="Dump the cache")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    subparser = parser.add_subparsers(help='sub-command help')
    build_command(subparser)
    cache_command(subparser)
    parser.parse_args()
