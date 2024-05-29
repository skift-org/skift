import os
import logging
import tempfile

from typing import Protocol
from cutekit import shell, const
from pathlib import Path


class Storage(Protocol):
    def store(self, src: Path, dest: Path) -> None: ...

    def mkdir(self, path: Path): ...

    def write(self, data: bytes, dest: Path) -> None: ...

    def finalize(self) -> Path: ...


class Dir(Storage):
    _root: Path

    def __init__(self, id: str):
        self._root = const.PROJECT_CK_DIR / "images" / f"{id}.dir"

    def store(self, src: Path, dest: Path) -> None:
        dest = self._root / dest
        shell.mkdir(Path(dest).parent)
        if src.is_dir():
            shell.cpTree(src, dest)
        else:
            shell.cp(src, dest)

    def mkdir(self, path: Path):
        path = self._root / path
        path.mkdir(parents=True, exist_ok=True)

    def write(self, data: bytes, dest: Path) -> None:
        dest = self._root / dest
        dest.parent.mkdir(parents=True, exist_ok=True)
        with dest.open("wb") as f:
            f.write(data)

    def finalize(self) -> Path:
        return self._root


class RawHdd(Storage):
    _hdd: Path
    _size: int
    _logger: logging.Logger

    def __init__(self, id: str, size: int):
        self._logger = logging.getLogger(f"HddStorage({id})")
        self._hdd = const.PROJECT_CK_DIR / "images" / f"{id}.hdd"

        if self._hdd:
            shell.rmrf(self._hdd)

        Path(self._hdd).parent.mkdir(parents=True, exist_ok=True)
        shell.exec(
            "dd", "if=/dev/zero", "bs=1M", "count=0", f"seek={size}", f"of={self._hdd}"
        )
        shell.exec("sgdisk", self._hdd, "-n", "1:2048", "-t", "1:ef00")
        shell.exec("mformat", "-i", self._hdd)

    def store(self, src: Path, dest: Path) -> None:
        dest.parent.mkdir(parents=True, exist_ok=True)
        try:
            shell.exec(
                "mcopy", "-D", "s", "-n", "-s", "-i", self._hdd, src, f"::{dest}"
            )
        except Exception:
            self._logger.warning(f"File {dest} already exists")

    def mkdir(self, path: Path):
        try:
            shell.exec("mmd", "-D", "s", "-i", self._hdd, f"::{path}")
        except Exception:
            self._logger.warning(f"Directory {path} already exists")

    def write(self, data: bytes, dest: Path) -> None:
        # create a temporary file
        with tempfile.NamedTemporaryFile() as tmp:
            tmp.write(data)
            tmp.flush()
            self.store(Path(tmp.name), dest)

    def finalize(self) -> Path:
        return self._hdd
