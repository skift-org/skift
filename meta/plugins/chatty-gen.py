# This is a hackish plugin for geneating header file from chatty interface
# definition. This should be replaced by a proper generator in the future when
# CuteKit supports more advanced rules.

from cutekit import shell, utils, ensure

ensure((0, 6, 0))

idlFiles = shell.find("src/", ["*.idl"])

for idlFile in idlFiles:
    headerFile = idlFile.replace(".idl", ".h")
    if utils.isNewer(idlFile, headerFile):
        shell.exec("cute-engineering-chatty", idlFile, headerFile)
        shell.exec("clang-format", "-i", headerFile)
