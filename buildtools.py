#!/usr/bin/python3

# Copyright © 2018-2019 MAKER.                                                #
# This code is licensed under the MIT License.                                #
# See: LICENSE.md                                                             #

# buildtools.py: the skift os build system.

import shutil
import subprocess
import json
import sys
import os
from enum import Enum

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

OBJDUMP = "./toolchain/local/bin/i686-elf-objdump"
GCC = "./toolchain/local/bin/i686-elf-gcc"
LD = "./toolchain/local/bin/i686-elf-ld"
AR = "./toolchain/local/bin/i686-elf-ar"

CFLAGS = ["-fno-pie", "-fno-builtin",
          "-ffreestanding", "-nostdlib", "-std=gnu11", "-nostdinc"]
CFLAGS_OPTIMIZATION = ["-O0", "-O1", "-O2", "-O3"]
CFLAGS_STRICT = ["-Wall", "-Wextra", "-Werror", "-Wno-unknown-pragmas"]

LDFLAGS = ["-flto"]
ASFLAGS = ["-f", "elf32"]

QEMUFLAGS = ["-sdl", "-m", "256M", "-serial", "mon:stdio", "-enable-kvm"]
QEMUFLAGS_NOKVM = ["-sdl", "-m", "256M", "-serial", "mon:stdio"]

def getoutput(command):
    result = subprocess.run(command, stdout=subprocess.PIPE)
    return result.stdout.decode("utf-8").strip()

GIT_REV = getoutput(["git", "rev-parse", "--short", "HEAD"])

def MKDIR(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

    return directory


def RMDIR(directory):
    if os.path.exists(directory):
        shutil.rmtree(directory)

def COPY(src, dest):
    if os.path.isdir(src):
        copytree(src, dest)
    else:
        shutil.copyfile(src, dest)

    return dest

def QEMU(disk):
    if subprocess.call(["qemu-system-i386", "-cdrom", disk] + QEMUFLAGS) != 0:
        if subprocess.call(["qemu-system-i386", "-cdrom", disk] + QEMUFLAGS_NOKVM) != 0:
            ERROR("Failed to start QEMU!")
            ABORT()

def GRUB(iso, output_file):
    with open("/dev/null", "w") as f:
        try:
            return 0 == subprocess.call(["grub-mkrescue", "-o", output_file, iso], stdout=f, stderr=f)
        except:
            print("grub-mkrescue not found, fallback grub2-mkrescue...")
            return 0 == subprocess.call(["grub2-mkrescue", "-o", output_file, iso], stdout=f, stderr=f)

    return False

def TAR(directory, output_file):
    subprocess.call(["tar", "-cf", output_file, "-C",
                     directory] + os.listdir(directory))

def ERROR(msg):
    print(BRIGHT_RED + "\nERROR: " + RESET + msg)

def ABORT():
    print("Aborted!" + RESET)
    exit(-1)

# --- Crosscompiler ---------------------------------------------------------- #

def crosscompiler_check():
    """
    Check if the cross compiler is present.
    """
    return os.path.exists(GCC) and \
        os.path.exists(LD)

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

    if not os.path.exists(outfile):
        return False
    else:
        if type(infiles) == list:
            uptodate = 0

            for i in infiles:
                if is_uptodate(outfile, i):
                    uptodate = uptodate + 1

            return len(infiles) == uptodate

        else:
            return os.path.getmtime(outfile) > os.path.getmtime(infiles)


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

def copytree(src, dst, ignore=None):
    if os.path.isdir(src):
        if not os.path.isdir(dst):
            os.makedirs(dst)
        files = os.listdir(src)

        if ignore is not None:
            ignored = ignore(src, files)
        else:
            ignored = set()

        for f in files:
            if f not in ignored:
                copytree(os.path.join(src, f),
                         os.path.join(dst, f),
                         ignore)
    else:
        shutil.copyfile(src, dst)

# --- Targets ---------------------------------------------------------------- #


class TargetTypes(Enum):
    INVALID = 0
    LIB = 1
    SHARED = 2
    APP = 3
    KERNEL = 4

    @staticmethod
    def FromStr(s):
        if s == "lib":
            return TargetTypes.LIB
        elif s == "app":
            return TargetTypes.APP
        elif s == "kernel":
            return TargetTypes.KERNEL
        elif s == "shared":
            return TargetTypes.SHARED
        else:
            return TargetTypes.INVALID


class Target(object):
    """
    Target building class.
    """

    def __init__(self, location, data):
        self.name = data["id"]
        self.name_friendly = data["name"] if "name" in data else data["id"]
        self.type = TargetTypes.FromStr(data["type"])
        self.deps = data["libs"] if "libs" in data else []
        self.incl = data["includes"] if "includes" in data else []
        self.strict = data["strict"] if "strict" in data else True
        
        self.location = location

        self.builded = False

    def get_dependancies_internal(self, targets, found=None, with_includes=False):
        if found == None:
            found = []

        for d in self.deps + (self.incl if with_includes else []):
            if d in targets:
                dep = targets[d]

                if not dep in found:
                    found.append(dep)
                    dep.get_dependancies_internal(targets, found)

            else:
                ERROR("Missing dependancies %s of target %s." % (d, self.name))
                ABORT()

        return found

    def get_dependancies(self, targets):
        dependancies = self.get_dependancies_internal(targets)

        if self in dependancies:
            dependancies.remove(self)

        return dependancies

    def get_libraries(self, targets):
        libraries = []

        for dep in self.get_dependancies(targets):
            if dep.type == TargetTypes.LIB:
                libraries.append(dep.get_output())

        return libraries

    def get_includes(self, targets):
        includes = [join(self.location, "includes")]

        for dep in self.get_dependancies(targets):
            includes.append(join(dep.location, "includes"))

        return includes

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

    def get_shared_objects(self, targets):

        objects = []

        for d in self.deps:
            if d in targets:
                dep = targets[d]

                if dep.type == TargetTypes.SHARED:
                    for s in dep.get_sources():
                        objects.append(s.replace(join(dep.location, "sources"),
                                                 join(dep.location, "obj")) + ".o")

            else:
                ERROR("Missing dependancies %s of target %s." % (d, self.name))
                ABORT()

        return objects

    def get_objects(self):
        objects = []

        for s in self.get_sources():
            objects.append(s.replace(join(self.location, "sources"),
                                     join(self.location, "obj")) + ".o")

        return objects

    def is_uptodate(self):
        return is_uptodate(self.get_output(), self.get_sources())

    # --- Target actions ----------------------------------------------------- #

    def compile(self, source, output, targets):
        """
        Compile a source file of the current target.
        """
        filename = source.split("/")[-1]
        includes = [("-I" + i) for i in self.get_includes(targets)]
        preprocessor = ["-MD", "-D__FILENAME__=\"" + filename + '"', \
                               "-D__PACKAGE__=\"" + self.name + '"', \
                               "-D__COMMIT__=\"" + GIT_REV + '"']

        print("    " + BRIGHT_BLACK + "%s" % filename + RESET)
        MKDIR(os.path.dirname(output))
        if source.endswith(".c"):
            command =                                    \
                [GCC] +                                  \
                preprocessor +                           \
                includes +                               \
                CFLAGS +                                 \
                (CFLAGS_STRICT if self.strict else []) + \
                [CFLAGS_OPTIMIZATION[3]] +               \
                ["-c", "-o", output, source]             \

        elif source.endswith(".s"):
            command = ["nasm", "-f" "elf32", source, "-o", output]

        return subprocess.call(command) == 0

    def link(self, targets):
        """
        Link the target
        """
        output_file = self.get_output()
        objects_files = self.get_objects()
        objects_files = objects_files + self.get_shared_objects(targets)

        print("    " + BRIGHT_WHITE + "Linking " + RESET + output_file)

        if self.type in [TargetTypes.APP, TargetTypes.KERNEL]:
            script = "./common/kernel.ld" if self.type == TargetTypes.KERNEL else "./common/userspace.ld"
            dependancies = self.get_libraries(targets)
            command = [LD] + LDFLAGS + ["-T", script, "-o", output_file] + \
                objects_files + dependancies
        elif self.type in [TargetTypes.LIB, TargetTypes.SHARED]:
            command = [AR, "rcs"] + [output_file] + objects_files
        else:
            print("No linking required, skipping...")
            return True

        if subprocess.call(command) == 0:
            with open(output_file + ".asm", "w") as f:
                command = [OBJDUMP, "-Mintel", "-S", output_file]
                return subprocess.call(command, stdout=f) == 0

        return False

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
                ERROR("Failed to build " + YELLOW + "%s " % (dep.name))
                return False

        if is_uptodate(self.get_output(), self.get_sources() + [dep.get_output() for dep in self.get_dependancies(targets)]):
            self.builded = True
            # print(BRIGHT_WHITE + self.name + RESET + " is up-to-date")
            return True
        else:
            print("")  # Skip a line so it's easier on the eyes.
            print(BRIGHT_WHITE + "%s:" % self.name_friendly + RESET)

            # Build all source file of the current target
            succeed = 0
            failed = 0

            for src, obj in zip(self.get_sources(), self.get_objects()):
                if not is_uptodate(obj, src):
                    if self.compile(src, obj, targets):
                        succeed += 1
                    else:
                        failed += 1
                        ERROR("Failed to build " +
                              BRIGHT_WHITE + "'%s'" % (src))

            print("    %d %s builded, %s%d%s succeed and %s%d%s failed.\n" % (succeed + failed,
                                                                              "files" if succeed + failed > 1 else "file", BRIGHT_GREEN, succeed, RESET, BRIGHT_RED, failed, RESET))

            # Link and output the result of the target
            return False if failed > 0 else self.link(targets)

    def clean(self):
        RMDIR(join(self.location, "bin"))
        RMDIR(join(self.location, "obj"))

# --- Target actions --------------------------------------------------------- #


def list_targets(location):
    """
    List all targets in a given folder.
    """

    targets = {}

    if os.path.isdir(location):
        for i in os.listdir(location):
            target_location = join(location, i)

            json_file = join(target_location, "manifest.json")

            if (os.path.exists(json_file)):
                data = json.loads(open(json_file).read())

                if (data["type"] == "compound"):
                    targets = {**targets, **list_targets(target_location)}
                else:
                    targets[data["id"]] = Target(target_location, data)

    return targets


# --- Action ----------------------------------------------------------------- #

def clean(target, targets):
    """Clean a target."""
    target.clean()


def build(target, targets):
    """Build a target."""

    target.build(targets)


def rebuild(target, targets):
    """Clean and build a target."""
    target.clean()
    target.build(targets)


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
        "info": info
    }


def clean_all(targets):
    """Clean all targets."""
    for t in targets:
        target = targets[t]
        target.clean()

    RMDIR("build")


def build_all(targets):
    """Build all tagets."""

    for t in targets:
        target = targets[t]
        if not target.build(targets):
            ERROR("Failed to build " + YELLOW + "%s " % (t))
            ABORT()


def rebuild_all(targets):
    """Clean and build all targets."""

    clean_all(targets)
    build_all(targets)


def distrib(targets):
    """Generate a distribution file."""

    build_all(targets)

    MKDIR("build")

    ramdisk = MKDIR("build/ramdisk")
    COPY("files", ramdisk)
    bootdisk = MKDIR("build/bootdisk")

    ## --- RAMDISK ---------------------------------------------------------- ##

    if not is_uptodate("build/ramdisk.tar", [targets[t].get_output() for t in targets if targets[t].type == TargetTypes.APP]):

        print(BRIGHT_WHITE + "\nGenerating ramdisk:" + RESET)

        app_dir = MKDIR(join(ramdisk, "bin"))

        for t in targets:
            target = targets[t]
            if target.type == TargetTypes.APP:
                print(BRIGHT_WHITE + "    Copying " +
                      RESET + "application '%s'" % t)
                COPY(target.get_output(), join(app_dir, target.name))

        print(BRIGHT_WHITE + "    Generating" + RESET + " the tarball")
        TAR(ramdisk, "build/ramdisk.tar")

    else:
        print("\n" + BRIGHT_WHITE + "Skipping" + RESET + " ramdisk")

    ## --- BOOTDISK --------------------------------------------------------- ##

    if not is_uptodate("build/bootdisk.iso", ["common/grub.cfg", targets["skift.hjert.kernel"].get_output(), "build/ramdisk.tar"]):
        print(BRIGHT_WHITE + "\nGenerating bootdisk:" + RESET)

        bootdir = MKDIR("build/bootdisk/boot")
        grubdir = MKDIR("build/bootdisk/boot/grub")

        COPY("common/grub.cfg", join(grubdir, "grub.cfg"))

        print(BRIGHT_WHITE + "    Copying" + RESET + " the kernel")
        COPY(targets["skift.hjert.kernel"].get_output(),
             join(bootdir, "kernel.bin"))

        print(BRIGHT_WHITE + "    Copying" + RESET + " the ramdisk")
        COPY("build/ramdisk.tar", join(bootdir, "ramdisk.tar"))

        print(BRIGHT_WHITE + "    Generating" + RESET + " the ISO")
        if not GRUB(bootdisk, "build/bootdisk.iso"):
            ERROR(
                "Failled to generate bootdisk... (check if xorriso or mtools is installed)")
            ABORT()

    else:
        print("\n" + BRIGHT_WHITE + "Skipping" + RESET + " bootdisk")

    # print(BRIGHT_YELLOW + "\nDistribution succeed 👌 !" + RESET)

def help_command(targets):
    """Show this help message."""

    print(BRIGHT_WHITE + "Build tools, version 3.0" + RESET)
    print("The skiftOS build system")

    print("")

    print(BRIGHT_WHITE + "Usage :" + RESET +
          " %s [action] targets..." % APP_NAME)
    print("        %s [global action]" % APP_NAME)
    print("")

    print(BRIGHT_WHITE + "Targets:" + RESET)
    for t in targets.keys():
        print("    %s" % (t))

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
    """List everithing else."""

    excluded = [TargetTypes.APP, TargetTypes.LIB]
    result = [i for i in targets if not targets[i].type in excluded]
    print(BRIGHT_WHITE + "Other: " + RESET + ', '.join(result))


def run_command(targets):
    """Start skiftOS in QEMU."""
    distrib(targets)

    print("")
    print(BRIGHT_WHITE + "Starting VM..." + RESET)
    QEMU("build/bootdisk.iso")


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
        "distrib": distrib,
        "run": run_command
    }

# --- Main ------------------------------------------------------------------- #


def missing_command(command):
    ERROR("No action named '%s'!" % command)
    print(BRIGHT_WHITE + "See: " + RESET + "'" + APP_NAME + " help'.")

def main(argc, argv):
    """
    Entry point of the SOSBS toolset.
    """
    targets = list_targets("packages")

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


if not crosscompiler_check():
    ERROR("Toolchain not found!")
    print("Falling back to the system toolchain (WARNING! this is not supported)")

    GCC = "gcc"
    LD = "ld"
    AR = "ar"
    OBJDUMP = "objdump"

    CFLAGS  += ["-m32", "-fno-stack-protector", "-mno-sse"]
    LDFLAGS += ["-m", "elf_i386"]


# Jump to the entry point.
if __name__ == '__main__':
    main(len(sys.argv), sys.argv)

print(RESET)
