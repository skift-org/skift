import os

from osdk import shell, builder, const
from osdk.cmds import Cmd, append
from osdk.args import Args
from osdk.logger import Logger
from pathlib import Path
from typing import Callable
import json
import magic


def kvmAvailable() -> bool:
    if os.path.exists("/dev/kvm") and \
            os.access("/dev/kvm", os.R_OK):
        return True
    return False


class Image:
    logger: Logger
    root: str

    def __init__(self, id: str):
        self.logger = Logger(f"image:{id}")
        self.root = f".osdk/images/{id}"

    def withPak(self, id: str, f: Callable[[dict], None]):
        jsonPath = f"{self.root}/bundles/{id}.json"
        pakJson = {
            "id": id,
            "objects": {}}
        if os.path.exists(jsonPath):
            pakJson = json.load(open(jsonPath))
        f(pakJson)
        json.dump(pakJson, open(jsonPath, "w+"), indent=4)

    def cpRef(self, pak: str, src: str, id: str) -> str:
        sha256 = shell.sha256sum(src)
        dest = f"objects/{sha256}"

        if not os.path.exists(f"{self.root}/{dest}"):
            self.cp(src, dest)

        mime = magic.from_file(f"{self.root}/{dest}", mime=True)

        def addToRefs(pakJson: dict):
            pakJson["objects"][id] = {"mime": mime, "ref": dest}

        self.withPak(pak, addToRefs)

        return dest

    def installTo(self, componentSpec: str, targetSpec: str, dest: str):
        self.logger.log(f"Installing {componentSpec} to {dest}...")
        component = builder.build(componentSpec, targetSpec)
        shell.mkdir(Path(f"{self.root}/{dest}").parent)
        shell.cp(component.outfile(), f"{self.root}/{dest}")

    def install(self, componentSpec: str, targetSpec: str):
        self.logger.log(f"Installing {componentSpec}...")
        component = builder.build(componentSpec, targetSpec)
        self.cpRef(componentSpec, component.outfile(), f"{componentSpec}/_bin")

    def cp(self, src: str, dest: str):
        self.logger.log(f"Copying {src} to {dest}...")
        shell.mkdir(Path(f"{self.root}/{dest}").parent)
        shell.cp(src, f"{self.root}/{dest}")

    def cpTree(self, src: str, dest: str):
        self.logger.log(f"Copying {src} to {dest}...")
        shell.mkdir(Path(f"{self.root}/{dest}").parent)
        shell.cpTree(src, f"{self.root}/{dest}")

    def mkdir(self, path: str):
        self.logger.log(f"Creating directory {path}...")
        shell.mkdir(f"{self.root}/{path}")

    def wget(self, url: str, dest: str):
        self.logger.log(f"Downloading {url} to {dest}...")
        shell.mkdir(Path(f"{self.root}/{dest}").parent)
        shell.wget(url, f"{self.root}/{dest}")


class Machine():
    logger: Logger

    def __init__(self, id: str):
        self.logger = Logger(f"machine:{id}")

    def boot(self, image: Image) -> None:
        pass


class QemuSystemAmd64(Machine):
    logError = False
    useDebug = False

    def __init__(self, logError: bool = False, useDebug: bool = False):
        super().__init__("qemu-system-x86_64")
        self.logError = logError
        self.useDebug = useDebug

    def boot(self, image: Image) -> None:
        self.logger.log("Booting...")

        ovmf = "/usr/share/edk2/x64/OVMF.fd"

        if not os.path.exists(ovmf):
            ovmf = shell.wget(
                "https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd")

        qemuCmd: list[str] = [
            "qemu-system-x86_64",
            "-machine", "q35",
            "-no-reboot",
            "-no-shutdown",
            "-serial", "mon:stdio",
            "-bios", ovmf,
            "-m", "256M",
            "-smp", "4",
            "-drive", f"file=fat:rw:{image.root},media=disk,format=raw",
            "-display", "sdl",
        ]

        if self.logError:
            qemuCmd += ["-d", "int,guest_errors,cpu_reset"]

        if self.useDebug:
            qemuCmd += ["-s", "-S"]

        if kvmAvailable():
            qemuCmd += ["-enable-kvm"]
        else:
            print("KVM not available, using QEMU-TCG")

        shell.exec(*qemuCmd)


def bootCmd(args: Args) -> None:
    image = Image("efi-x86_64")

    image.mkdir("objects")
    image.mkdir("bundles")

    image.installTo("loader", "efi-x86_64:o3", "EFI/BOOT/BOOTX64.EFI")

    image.install("hjert", "kernel-x86_64")
    image.install("limine-tests", "kernel-x86_64")
    image.install("system-srv", "skift-x86_64")
    image.install("shell-app", "skift-x86_64")
    image.cpTree("meta/image/boot", "boot/")

    machine = QemuSystemAmd64(logError=False, useDebug=False)
    machine.boot(image)


append(Cmd("s", "start", "Boot the system", bootCmd))
