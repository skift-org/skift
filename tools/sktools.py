"""skiftOS build and project managment toolbox"""

import json
import os
import shutil
import subprocess as sb
import sys
from enum import Enum

GCC_VERSION = 6.4.0
BINUTILS_VERSION = 

# +--------------------------------------------------------------------------+ #
# | Command line                                                             | #
# +--------------------------------------------------------------------------+ #

def main():
    check_and_compile_crosscompiler()
    pass

def clean():
    pass

def build():
    pass

# +--------------------------------------------------------------------------+ #
# | Utils                                                                    | #
# +--------------------------------------------------------------------------+ #

joinpath = os.path.join

def progress_bar(iteration, total, prefix = '', suffix = '', decimals = 1, length = 80, fill = '#'):
    """
    Call in a loop to create terminal progress bar

    Parameters
    ----------
    iteration : current iteration (int)
    total     : total iterations (int)
    prefix      - Optional  : prefix string (str)
    suffix      - Optional  : suffix string (str)
    decimals    - Optional  : positive number of decimals in percent complete (int)
    length      - Optional  : character length of bar (int)
    fill        - Optional  : bar fill character (str)
    """
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)

    print('\r%s [ %s ] %s%% %s' % (prefix, bar, percent, suffix), end = '\r')
    # Print New Line on Complete
    if iteration == total:
        print()

def uptodate(deps, out):
    """Check if the file is up to date with it dependencies."""

    if not os.path.exists(out):
        return False

    uptodate = True

    if isinstance(deps, str):
        uptodate = (os.path.getmtime(out) > os.path.getmtime(deps))
    else:
        for dep in deps:
            uptodate = uptodate and (os.path.getmtime(out) > os.path.getmtime(dep))

    return uptodate

# +--------------------------------------------------------------------------+ #
# | Shell                                                                    | #
# +--------------------------------------------------------------------------+ #

def FIND(dir, ext):
    files = []

    for root, directories, files in os.walk(dir):
        for file in files:
            if os.path.splitext(file)[1] in ext:
                files.append(os.path.join(root, file))

    return files

def MKDIR(dir):
    pass

def RMDIR(dir):
    pass

def COPY(src, dest):
    pass

def TAR(dir, out):
    pass

def UNTAR(file, out):
    pass

def MKISO(dir, out):
    pass

def WGET(url, out):
    return sb.call(["wget", "-O", out, url]) == 0

# +--------------------------------------------------------------------------+ #
# | Compiler                                                                 | #
# +--------------------------------------------------------------------------+ #

def check_crosscompiler():
    pass

def build_crosscompiler():
    pass

def check_and_compile_crosscompiler():
    if not check_crosscompiler():
        build_crosscompiler()

def NASM():
    pass

def GCC():
    pass

def AR():
    pass

def LD():
    pass

# +--------------------------------------------------------------------------+ #
# | Projects                                                                 | #
# +--------------------------------------------------------------------------+ #

class ProjectTypes(Enum):
    INVALID = 0
    LIB = 1
    APP = 2
    KERNEL = 3
    MODULE = 4

    @staticmethod
    def FromName(name):
        if name == "lib":
            return ProjectTypes.LIB
        elif name == "app":
            return ProjectTypes.APP
        elif name == "kernel":
            return ProjectTypes.KERNEL
        elif name == "module":
            return ProjectTypes.MODULE
        else:
            return ProjectTypes.INVALID

class Project(object):
    def __init__(self, path, data):
        self.path = path
        self.id = data["id"]
        self.type = ProjectTypes.FromName(data["type"])

        self.libs = data["libs"] if "libs" in data else []
        self.strict = data["strict"] if "strict" in data else True

    def get_sources(self):
        pass
