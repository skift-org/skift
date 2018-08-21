import json
import os
from enum import Enum

import toolchain
import utils
import datetime
import pprint


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
            return ProjectTypes.LIB
        else:
            return ProjectTypes.INVALID


class Project(object):
    def __init__(self, path, data):
        self.data = data

        if not "id" and "type" in data:
            self.valide = False
            return
        else:
            self.valide = True
            self.builded = False
            self.failed = False

            self.id = data["id"]
            self.type = ProjectTypes.FromName(data["type"])
            self.path = path

            if "libs" in data:
                self.libs = data["libs"]
            else:
                self.libs = []

            if "strict" in data:
                self.strict = data["strict"]
            else:
                self.strict = True

            self.bin_path = os.path.join(path, "bin")
            self.obj_path = os.path.join(path, "obj")

            self.assets_path = os.path.join(path, "assets")
            self.sources_path = os.path.join(path, "sources")
            self.includes_path = os.path.join(path, "includes")

            self.assets_file = os.path.join(self.obj_path, "__assets.s")

    def get_output(self):
        """
        Get the output file name of the current project.
        """
        file_name = self.id

        if (self.type == ProjectTypes.LIB):
            file_name += '.a'
        elif (self.type == ProjectTypes.APP):
            file_name += '.app'
        elif (self.type == ProjectTypes.KERNEL):
            file_name += '.bin'
        elif (self.type == ProjectTypes.MODULE):
            file_name += '.mod'

        return os.path.join(self.bin_path, file_name)

# --- Projects files --------------------------------------------------------- #

    def get_sources(self):
        """Get all sources files of the current project."""
        if os.path.exists(self.sources_path):
            return utils.GetFiles(self.sources_path, '.c')\
                 + utils.GetFiles(self.sources_path, '.s')
        
        return []

    def get_includes(self):
        """Get all includes files of the current project."""
        if os.path.exists(self.includes_path):
            return utils.GetFiles(self.includes_path, '.h')
        
        return []

    def get_assets(self):
        """Get all assets of the current project."""
        if os.path.exists(self.assets_path):
            return [os.path.join(self.assets_path, s) for s in os.listdir(self.assets_path)]
        
        return []

    def get_assets_filenames(self):
        if os.path.exists(self.assets_path):
            return os.listdir(self.assets_path)
        
        return []


    def get_includes_paths(self, projects):
        includes = [self.includes_path, self.obj_path]

        for lib in self.libs:
            includes.append(projects[lib].includes_path)

        return includes

    def get_objects(self):
        objects = []

        objects.append({"in": self.assets_file, "out": self.assets_file + '.o'})

        for src in self.get_sources():
            objects.append({"in": src, "out": src.replace(self.sources_path, self.obj_path) + '.o'})

        return objects

# --- Auto generated files --------------------------------------------------- #

    def generate_meta_header(self):
        """Generate '__meta.h' a header containg some build information from the project."""
        toolchain.MKDIR(self.obj_path)

        with open(os.path.join(self.obj_path, "__meta.h"), 'w') as f:
            f.write("#pragma once\n")
            f.write("// This file is auto generated.\n\n")

            f.write('#define __PROJECT_ID "%s"\n' % self.id)
            f.write('#define __PROJECT_TYPE "%s"\n' % self.type)
            f.write('#define __PROJECT_BUILD_DATE "%s"\n' % str(datetime.datetime.now()))

    def generate_assets_header(self):
        """Generate '__assets.h' a header contaning all built-in assets references."""
        toolchain.MKDIR(self.obj_path)

        with open(os.path.join(self.obj_path, "__assets.h"), 'w') as f:

            f.write("#pragma once\n")
            f.write("// This file is auto generated.\n")

            for a in self.get_assets_filenames():
                name = ("__" + self.id + "_"+ a).replace(".", "_")
                f.write("\n")
                f.write("extern const char %s_start;\n" % name)
                f.write("extern const char %s_end;\n" % name)
                f.write("extern const int %s_size;\n" % name)

    def generate_assets_assembly(self):
        """Generate '__assets.S' a assembly file contaning all built-in assets definition."""
        toolchain.MKDIR(self.obj_path)

        with open(os.path.join(self.obj_path, "__assets.s"), 'w') as f:
            
            f.write("bits 32\n")
            f.write("section .rodata\n")
            f.write(";; This file is auto generated.\n")

            f.write("global __assets_start\n")
            f.write("__assets_start:\n")

            for a in self.get_assets_filenames():
                name = ("__" + self.id + "_"+ a).replace(".", "_")

                f.write("\n")
                f.write("global %s_start\n" % name)
                f.write("global %s_end\n" % name)
                f.write("global %s_size\n" % name)

                f.write("\n")
                f.write("%s_start:   incbin \"%s\"\n" % (name, os.path.join(self.assets_path, a)))
                f.write("%s_end:\n" % name)
                f.write("%s_size:    dd $-%s_start\n" % (name, name))

            f.write("global __assets_end\n")
            f.write("__assets_end:\n")

    def generate_all(self):
        self.generate_meta_header()
        self.generate_assets_header()
        self.generate_assets_assembly()

# --- Building --------------------------------------------------------------- #

    def build_dependencies(self, projects):
        for l in self.libs:
            if not projects[l].build(projects):
                return False

        return True

    def build_objects(self, projects):
        objects = self.get_objects()

        for o in objects:
            os.makedirs(os.path.dirname(o["out"]), exist_ok=True)

            if o["in"].endswith(".s") and not toolchain.NASM(o["in"], o["out"]):
                self.failed = True
                return False
            elif o["in"].endswith(".c") and not toolchain.GCC(o["in"], o["out"], self.get_includes_paths(projects), [], self.strict):
                self.failed = True
                return False

        return True
        
    def link_output(self, projects):
        objects = [obj["out"] for obj in self.get_objects()]

        if self.type == ProjectTypes.APP:
            pass
        elif self.type == ProjectTypes.LIB:
            return toolchain.AR(objects, self.get_output())
        elif self.type == ProjectTypes.KERNEL:
            pass
        elif self.type == ProjectTypes.MODULE:
            pass
        return True

# --- Operations ------------------------------------------------------------- #

    def info(self):
        print("Project: %s" % self.id)
        print("Type: %s" % self.type)
        print("Output: %s" % self.get_output())
        print("Libs: %s" % ', '.join(self.libs))
        print("\nSources:")
        pprint.pprint(self.get_sources())
        print("\nAssets:")
        pprint.pprint(self.get_assets())
        print("\nObjects:")
        pprint.pprint(self.get_objects())

    def build(self, projects):
        toolchain.MKDIR(self.obj_path)
        toolchain.MKDIR(self.bin_path)

        if (self.builded):
            return True

        if (self.failed):
            return False

        self.generate_all()

        success = self.build_dependencies(projects)

        print("\nBuilding %s..." % self.id)
        success = success and self.build_objects(projects) and\
                              self.link_output(projects)

        self.builded = success
        return success

    def clean(self):
        toolchain.RMDIR(self.obj_path)
        toolchain.RMDIR(self.bin_path)
        