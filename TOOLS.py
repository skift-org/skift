#!/usr/bin/python3

import json
import os
import subprocess
import sys
import pprint
import shutil

# --- Usefull links ---------------------------------------------------------- #

"""
last modification: https://docs.python.org/3/library/path.html#path.getmtime
"""

def GetFiles(path, ext):
    files = []

    for root, directories, filenames in os.walk(path):
        for filename in filenames:
            if filename.endswith(ext):
                files.append(os.path.join(root, filename))

    return files

# --- Commands --------------------------------------------------------------- #


def MKDIR(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

def RMDIR(directory):
    shutil.rmtree(directory)

def GCC(file, includes):
    pass


def NASM():
    pass


def AR():
    pass


def LD(output, objects):
    pass

# --- Projects Managements --------------------------------------------------- #


class Project(object):
    def __init__(self, data):
        self.data = data

        if (not "id" in data or not "type" in data):
            self.valid = False
        else:
            self.valid = True
            self.builded = False

            self.id = data["id"]
            self.type = data["type"]
            self.path = data["path"]

            self.name = data["name"] if "name" in data else "Unamed"
            self.description = data["description"] if "description" in data else "No description."
            self.libs = data["libs"] if "libs" in data else []

    def GetAssets(self):
        assets_path = os.path.join(self.path, "assets")

        if os.path.exists(assets_path):
            return os.listdir(assets_path)

        return []

    def GetSources(self):
        sources_path = os.path.join(self.path, "sources")

        if os.path.exists(sources_path):
            c_sources = GetFiles(os.path.join(sources_path), '.c')
            s_sources = GetFiles(os.path.join(sources_path), '.s')

            return c_sources + s_sources
        
        return []

    def GetIncludes(self):
        includes_path = os.path.join(self.path, "includes")

        if os.path.exists(includes_path):
            return GetFiles(includes_path, '.h')

        return []

    def GetObjDir(self):
        return os.path.join(self.path, "obj")

    def GetBinDir(self):
        return os.path.join(self.path, "bin")

    def GetOutputFile(self):
        if (self.type == "lib"):
            return os.path.join(self.path, "%s.a", self.id)

        elif (self.type == "app"):
            return os.path.join(self.path, "%s.app", self.id)

        elif (self.type == "kernel"):
            return os.path.join(self.path, "kernel.bin")

    def GetDependencies(self, projects):
        dependancies = self.libs.copy()

        for deps in dependancies.copy():
            dependancies += projects[deps].libs

        return list(set(dependancies))

    # --- Operations --- #

    def Print(self, projects):
        print("")
        print("Project %s(%s):" % (self.name, self.id))
        pprint.pprint(self.data)
        print("Outpu:")
        pprint.pprint(self.GetOutputFile())
        print("Dependencies:")
        pprint.pprint(self.libs)
        print("All dependencies:")
        pprint.pprint(self.GetDependencies(projects))
        print("Includes:")
        pprint.pprint(self.GetIncludes())
        print("Sources:")
        pprint.pprint(self.GetSources())
        print("Assets:")
        pprint.pprint(self.GetAssets())

    def Clean(self):
        RMDIR(self.GetObjDir())
        RMDIR(self.GetBinDir())

    def Build(self, projects):
        obj_path = os.path.join(self.path, "obj")
        bin_path = os.path.join(self.path, "%s.")
        print("Building %s(%s): %s..." % (self.name, self.id, self.description))

def GetProjects(path):
    """
    Get all projects in a directory.

    Parameters
    ----------
    path: path to the directorie (str).

    Return
    ------
    A list of dictionary containing all project informations.
    """
    projects = {}

    for file in os.listdir(path):
        # Get project's paths
        project_path = os.path.join(os.getcwd(), file)
        json_path = os.path.join(project_path, "project.json")

        if os.path.isdir(project_path) and os.path.exists(json_path):
            # Load project's json.
            data = json.loads(open(json_path).read())
            data["path"] = project_path

            projects[data["id"]] = Project(data)

    return projects

def BuildAll(path):
    pass

# --- Build Script ----------------------------------------------------------- #


if __name__ == "__main__":
    projects = GetProjects(".")

    if len(sys.argv) == 3 and sys.argv[1] == "info":
        projects[sys.argv[2]].Print(projects)
    
    if len(sys.argv) == 2 and sys.argv[1] == "list":
        for project in projects:
            print(project, end=" ")
        print('')

    if len(sys.argv) == 2 and sys.argv[1] == "buildall":
        for id in projects:
            projects[id].Build(projects)

    if (len(sys.argv) == 1) or (len(sys.argv) == 2 and sys.argv[1] == "help"):
        print("skiftOS build system.")
