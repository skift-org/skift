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
AR = "./toolchain/local/bin/i686-elf-ar"

CFLAGS = ["-fno-pie", "-ffreestanding", "-nostdlib", "-std=gnu11", "-nostdinc"]
CFLAGS_OPTIMIZATION = ["-O0", "-O1", "-O2", "-O3"]
CFLAGS_STRICT = ["-Wall", "-Wextra", "-Werror"]

LDFLAGS = []
ASFLAGS = ["-f", "elf32"]

QEMUFLAGS = ["-display", "sdl", "-m", "256M",
             "-serial", "mon:stdio", "-M", "accel=kvm:tcg"]


def QEMU(disk):
    subprocess.call(["qemu-system-i386", "-cdrom", disk] + QEMUFLAGS)


def MKDIR(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

    return directory


def RMDIR(directory):
    if os.path.exists(directory):
        shutil.rmtree(directory)


def COPY(src, dest):
    shutil.copyfile(src, dest)


def TAR(directory, output_file):
    subprocess.call(["tar", "-cf", output_file, "-C",
                     directory] + os.listdir(directory))


def GRUB(iso, output_file):
    try:
        return 0 == subprocess.call(["grub-mkrescue", "-o", output_file, iso])
    except:
        print("grub-mkrescue not found, fallback grub2-mkrescue...")
        return 0 == subprocess.call(["grub2-mkrescue", "-o", output_file, iso])

    return False


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
    if subprocess.call('./toolchain/build-it!.sh', shell=True) != 0:
        ERROR("Building toolchain failed!")
        ABORT()

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

    return os.path.exists(outfile) and (os.path.getmtime(outfile) > os.path.getmtime(infiles))


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
        self.strict = data["strict"] if "strict" in data else True

        self.builded = False

    def get_dependancies(self, targets):
        dependancies = self.get_dependancies_internal(targets)

        if self in dependancies:
            dependancies.remove(self)

        return dependancies

    def get_dependancies_internal(self, targets, found=None):
        if found == None:
            found = []

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

        type_to_ext = {TargetTypes.LIB: ".lib",
                       TargetTypes.APP: ".app",
                       TargetTypes.KERNEL: ".bin"}

        if self.type in type_to_ext:
            return join(self.location, "bin/" + self.name + type_to_ext[self.type])
        else:
            return join(self.location, "bin/" + self.name + ".out")

    def get_sources(self):
        """
        Return all source file of the current target.
        """
        return get_files(join(self.location, "sources"), "c") + get_files(join(self.location, "sources"), "s")

    def get_objects(self):
        objects = []

        for s in self.get_sources():
            objects.append(s.replace(join(self.location, "sources"),
                                     join(self.location, "obj")) + ".o")

        return objects

    def get_includes(self, targets):

        deps = self.get_dependancies(targets)

        includes = [join(self.location, "includes")]

        for d in deps:
            includes.append(join(d.location, "includes"))

        return includes

    def is_uptodate(self):
        return is_uptodate(self.get_output(), self.get_sources())

    # --- Target actions ----------------------------------------------------- #

    def compile(self, source, output, targets):
        """
        Compile a source file of the current target.
        """
        if not is_uptodate(output, source):
            MKDIR(os.path.dirname(output))

            print("    " + BRIGHT_BLACK + "%s" % source + RESET)

            if source.endswith(".c"):
                includes = [("-I" + i) for i in self.get_includes(targets)]
                command = [GCC] + [CFLAGS_OPTIMIZATION[3]] + CFLAGS + includes + (CFLAGS_STRICT if self.strict else []) + ["-c", "-o", output, source]
            elif source.endswith(".s"):
                command = ["nasm", "-f" "elf32", source, "-o", output]

            return subprocess.call(command) == 0

        return True

    def link(self, targets):
        """
        Link the target
        """
        output_file = self.get_output()
        objects_files = self.get_objects()

        print("    " + BRIGHT_WHITE + "Linking " + RESET + output_file)

        if self.type in [TargetTypes.APP, TargetTypes.KERNEL]:
            script = "./common/kernel.ld" if self.type == TargetTypes.KERNEL else "./common/userspace.ld"
            dependancies = [dep.get_output() for dep in self.get_dependancies(targets)]
            command = [LD, "-T", script, "-o", output_file] + objects_files + dependancies
        elif self.type == TargetTypes.LIB:
            command = [AR, "rcs"] + [output_file] + objects_files

        pprint(command)
        print(" ".join(command))
        return subprocess.call(command) == 0

    def build(self, targets):
        """
        Build source files and link the target.
        """

        if self.builded:
            return True
        else:
            self.builded = True

        MKDIR(join(self.location, "bin"))
        MKDIR(join(self.location, "obj"))

        for dep in self.get_dependancies(targets):
            if not dep.build(targets):
                ERROR("Failed to build " + YELLOW + "%s " %
                      (dep.name))
                return False

        # Skip a line so it's easier on the eyes.
        print("")
        print(BRIGHT_WHITE + "%s:" % self.name + RESET)

        # Build all source file of the current target
        for src, obj in zip(self.get_sources(), self.get_objects()):
            if not self.compile(src, obj, targets):
                ERROR("Failed to build " + BRIGHT_WHITE + "'%s'" % (src))
                return False

        # Link and output the result of the target
        return self.link(targets)

# ---- Distribution ---------------------------------------------------------- #


def generate_ramdisk(targets):
    pass


def generate_bootdisk(targets):
    pass

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


def distrib(targets):
    """Generate a distribution file."""
    pass


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

    result = [i for i in targets if targets[i].type == TargetTypes.LIB]
    print(BRIGHT_WHITE + "Libraries: " + RESET + ', '.join(result))


def list_app(targets):
    """List all available applications."""

    result = [i for i in targets if targets[i].type == TargetTypes.APP]
    print(BRIGHT_WHITE + "Applications: " + RESET + ', '.join(result))


def list_other(targets):
    """List all everithing else."""

    excluded = [TargetTypes.APP, TargetTypes.LIB]
    result = [i for i in targets if not targets[i].type in excluded]
    print(BRIGHT_WHITE + "Other: " + RESET + ', '.join(result))


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
            crosscompiler_build()
        else:
            ABORT()

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
