import os
import logging
import tempfile

from typing import Protocol
from cutekit import shell, const
from pathlib import Path


class Storage(Protocol):
    def store(self, src: str, dest: str) -> None: ...

    def mkdir(self, path: str): ...

    def write(self, data: bytes, dest: str) -> None: ...

    def finalize(self) -> str: ...


class Dir(Storage):
    _root: str

    def __init__(self, id: str):
        self._root = f"{const.PROJECT_CK_DIR}/images/{id}.dir"

    def store(self, src: str, dest: str) -> None:
        dest = os.path.join(self._root, dest)
        shell.mkdir(Path(dest).parent)
        if os.path.isdir(src):
            shell.cpTree(src, dest)
        else:
            shell.cp(src, dest)

    def mkdir(self, path: str):
        path = os.path.join(self._root, path)
        shell.mkdir(path)

    def write(self, data: bytes, dest: str) -> None:
        dest = os.path.join(self._root, dest)
        shell.mkdir(Path(dest).parent)
        with open(dest, "wb+") as f:
            f.write(data)

    def finalize(self) -> str:
        return self._root

    def clean(self) -> None:
        shell.rmrf(self._root)


class BootFs(Dir):
    _out: str

    def __init__(self, id: str):
        super().__init__(id)
        self._out = f"{const.PROJECT_CK_DIR}/images/{id}.bootfs"

    def finalize(self) -> str:
        print("Generating bootfs...")
        BOOTFS = ".cutekit/extern/cute-engineering/ce-bootfs/src/bootfs.py"
        shell.exec("python", BOOTFS, "--dir", self._root, self._out)
        return self._out


class RawHdd(Storage):
    _hdd: str
    _size: int
    _logger: logging.Logger

    def __init__(self, id: str, size: int):
        self._logger = logging.getLogger(f"HddStorage({id})")
        self._hdd = f"{const.PROJECT_CK_DIR}/images/{id}.hdd"

        if os.path.exists(self._hdd):
            shell.rmrf(self._hdd)

        shell.mkdir(Path(self._hdd).parent)
        shell.exec(
            "dd", "if=/dev/zero", "bs=1M", "count=0", f"seek={size}", f"of={self._hdd}"
        )
        shell.exec("sgdisk", self._hdd, "-n", "1:2048", "-t", "1:ef00")
        shell.exec("mformat", "-i", self._hdd)

    def store(self, src: str, dest: str) -> None:
        self.mkdir(Path(dest).parent)
        try:
            shell.exec(
                "mcopy", "-D", "s", "-n", "-s", "-i", self._hdd, src, f"::{dest}"
            )
        except Exception:
            self._logger.warning(f"File {dest} already exists")

    def mkdir(self, path: str):
        try:
            shell.exec("mmd", "-D", "s", "-i", self._hdd, f"::{path}")
        except Exception:
            self._logger.warning(f"Directory {path} already exists")

    def write(self, data: bytes, dest: str) -> None:
        # create a temporary file
        with tempfile.NamedTemporaryFile() as tmp:
            tmp.write(data)
            tmp.flush()
            self.store(tmp.name, dest)

    def finalize(self) -> str:
        return self._hdd
