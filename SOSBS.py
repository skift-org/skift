#!/usr/bin/python3

"""
S.O.S.B.S: The (S)kift(O)(S) (B)uild (S)ystem
"""

from pprint import pprint
from enum import Enum

import os
import sys
import json
import subprocess
import shutil

# --- Utils ------------------------------------------------------------------ #

APP_NAME = sys.argv[0]

ESC = '\033['

BLACK = ESC + '30m'
RED = ESC + '31m'
GREEN = ESC + '32m'
YELLOW = ESC + '33m'
BLUE = ESC + '34m'
MAGENTA = ESC + '35m'
CYAN = ESC + '36m'
WHITE = ESC + '37m'

BRIGHT_BLACK = ESC + '30;1m'
BRIGHT_RED = ESC + '31;1m'
BRIGHT_GREEN = ESC + '32;1m'
BRIGHT_YELLOW = ESC + '33;1m'
BRIGHT_BLUE = ESC + '34;1m'
BRIGHT_MAGENTA = ESC + '35;1m'
BRIGHT_CYAN = ESC + '36;1m'
BRIGHT_WHITE = ESC + '37;1m'

RESET = ESC + '0m'

GCC = "./toolchain/local/bin/i686-elf-gcc"
LD = "./toolchain/local/bin/i686-elf-ld"

CFLAGS = ["-O3", "-fno-pie", "-ffreestanding",
          "-nostdlib", "-std=gnu11", "-nostdinc"]

LDFLAGS = []
ASFLAGS = ["-f", "elf32"]

QEMUFLAGS = ["-display", "sdl", "-m", "256M",
             "-serial", "mon:stdio", "-M", "accel=kvm:tcg"]


def ERROR(msg):
    print(BRIGHT_RED + "ERROR: " + RESET + msg)


def ABORT():
    print("Aborted!")
    exit(-1)


# --- Crosscompiler ---------------------------------------------------------- #

def crosscompiler_check():
    """
    Check if the cross compiler is present.
    """
    return os.path.exists(GCC) and \
        os.path.exists(LD)


def crosscompiler_build():
    """
    Build the cross compiler.
    """
    pass

# --- Utils ------------------------------------------------------------------ #


def join(a, b):
    """
    Joint to paths
    """
    return os.path.join(a, b)


def is_uptodate(outfile, infiles):
    """
    Check if a file is uptodate with its dependancies.
    """
    if type(infiles) == list:
        uptodate = 0

        for i in infiles:
            if is_uptodate(outfile, i):
                uptodate = uptodate + 1

        return len(infiles) == uptodate

    return os.path.exists(outfile) and (os.path.getmtime(output_file) > os.path.getmtime(input_file))


def get_files(locations, ext):
    """
    Return all files of a givent extention in specified folder.
    """
    files = []

    for root, _, filenames in os.walk(locations):
        for filename in filenames:
            if filename.endswith(ext):
                files.append(join(root, filename))

    return files

# --- Targets ---------------------------------------------------------------- #


class TargetTypes(Enum):
    INVALID = 0
    LIB = 1
    APP = 2
    KERNEL = 3
    MODULE = 4

    @staticmethod
    def FromStr(s):
        if s == "lib":
            return TargetTypes.LIB
        elif s == "app":
            return TargetTypes.APP
        elif s == "kernel":
            return TargetTypes.KERNEL
        elif s == "module":
            return TargetTypes.MODULE
        else:
            return TargetTypes.INVALID


class Target(object):
    """
    Target building class.
    """

    def __init__(self, location, data):
        self.name = data["id"]
        self.type = TargetTypes.FromStr(data["type"])
        self.location = location
        self.deps = data["libs"] if "libs" in data else []

    def get_dependancies(self, targets):
        dependancies = self.get_dependancies_internal(targets)

        dependancies.remove(self)

        return dependancies

    def get_dependancies_internal(self, targets, found=[]):
        for d in self.deps:
            if d in targets:
                dep = targets[d]

                if not dep in found:
                    found.append(dep)
                    dep.get_dependancies_internal(targets, found)

            else:
                ERROR("Missing dependancies %s of target %s." % (d, self.name))
                ABORT()

        return found

    def get_output(self):
        """
        Return the name of the output file of the current target.
        """
        if self.type == TargetTypes.LIB:
            return join(self.location, self.name + ".lib")
        elif self.type == TargetTypes.APP:
            return join(self.location, self.name + ".app")
        elif self.type == TargetTypes.KERNEL:
            return join(self.location, self.name + ".bin")

        return join(self.location, self.name + ".out")

    def get_sources(self):
        """
        Return all source file of the current target.
        """
        return get_files(join(self.location, "sources"), "c")

    def is_uptodate(self):
        return is_uptodate(self.get_output(), self.get_sources())

    # --- Target actions ----------------------------------------------------- #

    def compile(self, source):
        """
        Compile a source file of the current target.
        """
        print("    " + BRIGHT_BLACK + "%s" % source + RESET)

        if source.endswith(".c"):
            command = [GCC] + CFLAGS + ["-o", source + ".o", source]
            print(" ".join(command))
        elif source.endswith(".s"):
            pass

        return True

    def link(self, targets):
        """
        Link the target
        """
        print("    " + BRIGHT_WHITE + "Linking" +
              RESET + " %s" % self.get_output())

        return True

    def build(self, targets):
        """
        Build source files and link the target.
        """
        # Skip a line so it's easier on the eyes.
        print("")
        print(BRIGHT_WHITE + "%s:" % self.name + RESET)

        # Build all source file of the current target
        for source in self.get_sources():
            if not self.compile(source):
                return False

        # Link and output the result of the target
        return self.link(targets)


# --- Target actions --------------------------------------------------------- #

def list_targets(location):
    """
    List all targets in a given folder.
    """

    targets = {}

    for i in os.listdir(location):
        target_location = join(location, i)

        json_file = join(target_location, "manifest.json")

        if (os.path.exists(json_file)):
            data = json.loads(open(json_file).read())
            targets[data["id"]] = Target(target_location, data)

    return targets


# --- Action ----------------------------------------------------------------- #

def clean(target, targets):
    """Clean a target."""
    pass


def build(target, targets):
    """Build a target."""

    target.build(targets)


def rebuild(target, targets):
    """Clean and build a target."""
    pass


def run(target, targets):
    """Start the kernel and the specified target."""
    pass


def info(target, targets):
    """Dump information about the target."""

    print(BRIGHT_WHITE + "Target '" + target.name + "':" + RESET)
    print("\tType: " + str(target.type.name.lower()))
    print("\tUptodate: " + str(target.is_uptodate()))
    print("\tDirect Dependencies: " + ", ".join(target.deps))

    all_deps = target.get_dependancies(targets)
    print("\tAll dependencies: " + ", ".join([a.name for a in all_deps]))

    print("\tLocation: " + target.location)
    print("\tOutput: " + target.get_output())


actions = \
    {
        "build": build,
        "clean": clean,
        "rebuild": rebuild,
        "run": run,
        "info": info
    }


def clean_all(targets):
    """Clean all targets."""
    pass


def build_all(targets):
    """Build all tagets."""

    for t in targets:
        target = targets[t]
        if not target.build(targets):
            ERROR("'%s' compilation failed!" % t)
            ABORT()


def rebuild_all(targets):
    """Clean and build all targets."""

    clean_all(targets)
    build_all(targets)


def help_command(targets):
    """Show this help message."""

    print(BRIGHT_WHITE + "S.O.S.B.S, version 2.0" + RESET)
    print("The skiftOS build system")
    print("")

    print(BRIGHT_WHITE + "Usage :" + RESET +
          " %s [action] targets..." % APP_NAME)
    print("        %s [global action]" % APP_NAME)
    print("")

    print(BRIGHT_WHITE + "Targets:" + RESET)
    print("   ", ', '.join(targets.keys()))

    print("\n" + BRIGHT_WHITE + "Actions:" + RESET)
    for act in actions:
        print("    %-12s %s" % (act, actions[act].__doc__))

    print("\n" + BRIGHT_WHITE + "Global actions:" + RESET)
    for act in global_actions:
        print("    %-12s %s" % (act, global_actions[act].__doc__))


def list_command(targets):
    """List all available targets."""

    list_app(targets)
    list_lib(targets)
    list_other(targets)


def list_lib(targets):
    """List all available libraries."""

    libraries = {k: v for k, v in targets.items() if v.type == TargetTypes.LIB}
    print(BRIGHT_WHITE + "Libraries: " + RESET + ', '.join(libraries.keys()))


def list_app(targets):
    """List all available applications."""

    applications = {k: v for k, v in targets.items() if v.type ==
                    TargetTypes.APP}
    print(BRIGHT_WHITE + "Applications: " +
          RESET + ', '.join(applications.keys()))


def list_other(targets):
    """List all everithing else."""

    applications = {k: v for k, v in targets.items(
    ) if v.type != TargetTypes.APP and v.type != TargetTypes.LIB}
    print(BRIGHT_WHITE + "Other: " + RESET + ', '.join(applications.keys()))


global_actions = \
    {
        "build-all": build_all,
        "clean-all": clean_all,
        "help": help_command,
        "list": list_command,
        "list-app": list_app,
        "list-lib": list_lib,
        "list-other": list_other,
        "rebuild-all": rebuild_all,
    }

# --- Main ------------------------------------------------------------------- #


def missing_command(command):
    ERROR("No action named '%s'!" % command)
    print(BRIGHT_WHITE + "See: " + RESET + "'" + APP_NAME + " help'.")


def main(argc, argv):
    """
    Entry point of the SOSBS toolset.
    """
    targets = list_targets("sources")

    # Check and build the cross compiler if the user say 'YES'.
    if not crosscompiler_check():
        ERROR("Toolchain not found!")

        respond = input(
            "Would you like to build one (may take 5 to 15 minutes depending of your system)? [yes/no]\n > ")

        if respond in ['y', "yes", 'o', "oui"]:
            pass
        else:
            exit()

    # Command parsing

    if argc < 2:
        help_command(targets)
    else:
        action = argv[1]

        # Check if the actions is a valid one
        if action in actions:
            # Check if a target is specified
            if argc < 3:
                ERROR("No target specified!")
                ABORT()
            else:
                target = argv[2]

                # check  if the target is valid.
                if target in targets:
                    actions[action](targets[target], targets)
                else:
                    ERROR("No target named '%s'!" % target)
                    ABORT()

        elif action in global_actions:
            global_actions[action](targets)

        else:
            missing_command(action)


# Jump to the entry point.
if __name__ == '__main__':
    main(len(sys.argv), sys.argv)
