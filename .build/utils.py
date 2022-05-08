
import subprocess
import os
from typing import Optional


def mkdir(path: str) -> str:
    if not os.path.isdir(path):
        os.mkdir(path)
    return path


def rm(path: str) -> None:
    if os.path.exists(path):
        os.remove(path)


def popen(args: list[str]) -> tuple[int, str]:
    process = subprocess.Popen(args, stdout=subprocess.PIPE)
    (output, err) = process.communicate()
    code = process.wait()
    return (code, output.decode("utf-8").rstrip())


def make(*args: str) -> tuple[int, str]:
    return popen(["make", "-f", ".build/backend.mk", *args])


def git_branch() -> Optional[str]:
    (code, output) = popen(["git", "rev-parse", "--abbrev-ref", "HEAD"])
    return output if code == 0 else None


def git_commit() -> Optional[str]:
    (code, output) = popen(["git", "rev-parse", "HEAD"])
    return output if code == 0 else None
