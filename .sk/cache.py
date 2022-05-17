import utils
import hashlib
import os

CACHEDIR = utils.mkdir(".cache")
POPENCACHEDIR = utils.mkdir(".cache/popen")


def popen(args: list[str]) -> tuple[int, str]:
    key = "_".join(args)
    path = CACHEDIR + "/popen/" + \
        hashlib.sha256(key.encode("utf-8")).hexdigest()
    if not os.path.exists(path):
        (code, output) = utils.popen(args)
        if code == 0:
            with open(path, "w") as f:
                f.write(output)

        return (code, output)
    with open(path, "r") as f:
        return (0, f.read())


def make(*args: str) -> tuple[int, str]:
    return popen(["make", "-f", ".sk/build.mk", *args])


def nuke() -> None:
    utils.rm(CACHEDIR)
