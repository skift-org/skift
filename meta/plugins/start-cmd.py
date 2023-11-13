import os
import json
import tempfile
import magic
import logging
from pathlib import Path
from typing import Any, Protocol
from cutekit import shell, builder, const, model, cli, ensure

ensure((0, 6, 0))


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
    _registry: model.Registry
    _store: Storage
    _logger: logging.Logger
    _paks: dict[str, dict[str, Any]]
    _finalized: str | None

    def __init__(self, registry: model.Registry, store: Storage):
        self._registry = registry
        self._store = store
        self._logger = logging.getLogger("Image")
        self._paks = {}
        self._finalized = None

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
        product = self.install(componentSpec, targetSpec)
        self.cp(str(product.path), dest=dest)

    def install(self, componentSpec: str, targetSpec: str) -> builder.Product:
        self._logger.info(f"Installing {componentSpec}...")
        component = self._registry.lookup(componentSpec, model.Component)
        assert component is not None

        target = self._registry.lookup(targetSpec, model.Target)
        assert target is not None

        product = builder.build(target, self._registry, component)[0]

        for depId in component.resolved[target.id].resolved + [componentSpec]:
            dep = self._registry.lookup(depId, model.Component)
            if dep is None:
                raise Exception(f"Component {depId} not found")

            for res in builder.listRes(component):
                rel = Path(res).relative_to(component.subpath("res"))
                self.cpRef("_index", res, f"{depId}/{rel}")
                self.cpRef(componentSpec, res, f"{depId}/{rel}")

        self.cpRef("_index", str(product.path), f"{componentSpec}/_bin")
        self.cpRef(componentSpec, str(product.path), f"{componentSpec}/_bin")

        return product

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


def kvmAvailable() -> bool:
    if os.path.exists("/dev/kvm") and os.access("/dev/kvm", os.R_OK):
        return True
    return False


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


def generateSystem(image: Image) -> None:
    image.mkdir("objects")
    image.mkdir("bundles")

    image.mkdir("EFI")
    image.mkdir("EFI/BOOT")
    image.installTo("loader", "efi-x86_64", "EFI/BOOT/BOOTX64.EFI")

    image.install("hjert", "kernel-x86_64")

    image.install("grund-system", "skift-x86_64")
    image.install("grund-device", "skift-x86_64")
    image.install("hideo-shell", "skift-x86_64")
    image.install("skift-branding", "skift-x86_64")
    image.install("skift-wallpapers", "skift-x86_64")
    image.install("inter-font", "skift-x86_64")
    image.install("mdi-font", "skift-x86_64")

    image.cpTree("meta/image/boot", "boot")


@cli.command(None, "image", "Generate the boot image")
def imageCmd(args: cli.Args) -> None:
    debug = args.consumeOpt("debug", False) is True
    fmt = args.consumeOpt("format", "dir")
    args.opts["mixins"] = "debug" if debug else "o3"

    registry = model.Registry.use(args)

    store = (
        DirStorage("image-efi-x86_64")
        if fmt == "dir"
        else HddStorage("image-efi-x86_64", 256)
    )
    image = Image(registry, store)
    generateSystem(image)
    print(image.finalize())


@cli.command("s", "start", "Boot the system")
def bootCmd(args: cli.Args) -> None:
    debug = args.consumeOpt("debug", False) is True
    args.opts["mixins"] = "debug" if debug else "o3"

    registry = model.Registry.use(args)

    logError = args.consumeOpt("dint", False) is True

    image = Image(registry, DirStorage("image-efi-x86_64"))
    generateSystem(image)
    machine = QemuSystemAmd64(logError=logError, debugger=debug)
    machine.boot(image)
