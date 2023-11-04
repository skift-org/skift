import os
import json
import tempfile
import magic
import logging
from pathlib import Path


from cutekit import shell, builder, const, project, model
from cutekit.cmds import Cmd, append
from cutekit.args import Args
from typing import Any, Protocol


def kvmAvailable() -> bool:
    if os.path.exists("/dev/kvm") and os.access("/dev/kvm", os.R_OK):
        return True
    return False


class Storage(Protocol):
    def store(self, src: str, dest: str) -> None:
        ...

    def mkdir(self, path: str):
        ...

    def write(self, data: bytes, dest: str) -> None:
        ...

    def finalize(self) -> str:
        ...


class DirStorage(Storage):
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


class HddStorage(Storage):
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
            print("mcopy", "-D", "s", "-n", "-s", "-i", self._hdd, src, f"::{dest}")
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
        with tempfile.TemporaryFile() as tmp:
            tmp.write(data)
            tmp.flush()
            self.store(tmp.name, dest)

    def finalize(self) -> str:
        return self._hdd


class Image:
    _store: Storage
    _logger: logging.Logger
    _paks: dict[str, dict[str, Any]]
    _finalized: str | None
    _props: model.Props

    def __init__(self, store: Storage, props: model.Props):
        self._store = store
        self._logger = logging.getLogger("Image")
        self._paks = {}
        self._finalized = None
        self._props = props

    def ensurePak(self, id: str) -> dict[str, Any]:
        if id not in self._paks:
            self._paks[id] = {"id": id, "objects": {}}
        return self._paks[id]

    def cpRef(self, pak: str, src: str, id: str) -> str:
        ref = f"bundle://{id}"
        sha256 = shell.sha256sum(src)
        dest = f"objects/{sha256}"
        mime = magic.from_file(src, mime=True)
        self.ensurePak(pak)["objects"][ref] = {
            "mime": mime,
            "ref": f"file:/{dest}",
        }
        self._store.store(src, dest)
        return dest

    def installTo(self, componentSpec: str, targetSpec: str, dest: str):
        self._logger.info(f"Installing {componentSpec} to {dest}...")
        component = builder.build(componentSpec, targetSpec, self._props)
        context = component.context

        for depId in component.resolved + [componentSpec]:
            dep = context.componentByName(depId)
            if dep is None:
                raise Exception(f"Component {depId} not found")

            for asset in dep.resfiles():
                self.cpRef("_index", asset[0], f"{depId}/{asset[2]}")
                self.cpRef(componentSpec, asset[0], f"{depId}/{asset[2]}")

        self.cp(component.outfile(), dest)

    def install(self, componentSpec: str, targetSpec: str):
        self._logger.info(f"Installing {componentSpec}...")
        component = builder.build(componentSpec, targetSpec, self._props)
        context = component.context

        for depId in component.resolved + [componentSpec]:
            dep = context.componentByName(depId)
            if dep is None:
                raise Exception(f"Component {depId} not found")

            for asset in dep.resfiles():
                self.cpRef("_index", asset[0], f"{depId}/{asset[2]}")
                self.cpRef(componentSpec, asset[0], f"{depId}/{asset[2]}")

        self.cpRef("_index", component.outfile(), f"{componentSpec}/_bin")
        self.cpRef(componentSpec, component.outfile(), f"{componentSpec}/_bin")

    def cp(self, src: str, dest: str):
        self._logger.info(f"Copying {src} to {dest}...")
        self._store.store(src, dest)

    def cpTree(self, src: str, dest: str):
        self._logger.info(f"Copying {src} to {dest}...")
        self._store.store(src, dest)

    def mkdir(self, path: str):
        self._logger.info(f"Creating directory {path}...")
        self._store.mkdir(path)

    def finalize(self) -> str:
        if not self._finalized:
            for k, v in self._paks.items():
                self._store.write(json.dumps(v).encode("utf-8"), f"bundles/{k}.json")
            self._finalized = self._store.finalize()
        return self._finalized


class Machine:
    logger: logging.Logger

    def __init__(self, id: str):
        self._logger = logging.getLogger(f"Machine({id})")

    def boot(self, image: Image) -> None:
        pass


class QemuSystemAmd64(Machine):
    logError = False
    debugger = False

    def __init__(self, logError: bool = False, debugger: bool = False):
        super().__init__("qemu-system-x86_64")
        self.logError = logError
        self.debugger = debugger

    def boot(self, image: Image) -> None:
        self._logger.info("Booting...")

        ovmf = "/usr/share/edk2/x64/OVMF.fd"

        if not os.path.exists(ovmf):
            ovmf = shell.wget(
                "https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd"
            )

        qemuCmd: list[str] = [
            "qemu-system-x86_64",
            "-machine",
            "q35",
            "-no-reboot",
            "-no-shutdown",
            "-serial",
            "mon:stdio",
            "-bios",
            ovmf,
            "-m",
            "256M",
            "-smp",
            "4",
            "-drive",
            f"file=fat:rw:{image.finalize()},media=disk,format=raw",
            "-display",
            "sdl",
        ]

        if self.logError:
            qemuCmd += ["-d", "int,guest_errors,cpu_reset"]

        if self.debugger:
            qemuCmd += ["-s", "-S"]

        if kvmAvailable():
            if not self.logError:
                qemuCmd += ["-enable-kvm"]
        else:
            print("KVM not available, using QEMU-TCG")

        shell.exec(*qemuCmd)


def generateSystem(image: Image, debug: bool = False) -> None:
    image.mkdir("objects")
    image.mkdir("bundles")

    efiTarget = "efi-x86_64" + (":debug" if debug else ":o3")
    image.mkdir("EFI")
    image.mkdir("EFI/BOOT")
    image.installTo("loader", efiTarget, "EFI/BOOT/BOOTX64.EFI")

    kernelTarget = "kernel-x86_64" + (":debug" if debug else ":o3")
    image.install("hjert", kernelTarget)

    skiftTarget = "skift-x86_64" + (":debug" if debug else ":o3")
    image.install("grund-system", skiftTarget)
    image.install("grund-device", skiftTarget)
    image.install("hideo-shell", skiftTarget)
    image.install("skift-branding", skiftTarget)
    image.install("skift-wallpapers", skiftTarget)
    image.install("inter-font", skiftTarget)
    image.install("mdi-font", skiftTarget)

    image.cpTree("meta/image/boot", "boot")


def imageCmd(args: Args) -> None:
    project.chdir()

    debug = args.consumeOpt("debug", False)
    fmt = args.consumeOpt("format", "dir")
    props = args.consumePrefix("prop:")

    store = (
        DirStorage("image-efi-x86_64")
        if fmt == "dir"
        else HddStorage("image-efi-x86_64", 256)
    )
    image = Image(store, props)
    generateSystem(image, debug)
    print(image.finalize())


def bootCmd(args: Args) -> None:
    project.chdir()

    debug = args.consumeOpt("debug", False)
    logError = args.consumeOpt("dint", False)
    props = args.consumePrefix("prop:")

    image = Image(DirStorage("image-efi-x86_64"), props)
    generateSystem(image, debug)
    machine = QemuSystemAmd64(logError=logError, debugger=debug)
    machine.boot(image)


append(Cmd(None, "image", "Generate the boot image", imageCmd))
append(Cmd("s", "start", "Boot the system", bootCmd))
