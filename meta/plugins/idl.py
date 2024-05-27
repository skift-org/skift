from pathlib import Path
from cutekit import shell, utils, builder, const


@builder.hook("generate-idl-headers")
def _genIdlHook(scope: builder.TargetScope):
    for c in scope.components:
        idls = c.wilcard("*.idl")

        for idl in idls:
            relpath = Path(idl).relative_to(c.component.dirname())
            dirpath = Path(const.GENERATED_DIR) / c.component.id
            destpath = dirpath / relpath.with_suffix(".h")
            if utils.isNewer(idl, destpath):
                shell.mkdir(str(dirpath))
                shell.exec("cute-engineering-chatty", idl, destpath)
                shell.exec("clang-format", "-i", destpath)
