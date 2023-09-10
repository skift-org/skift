import os
import json
import magic
import logging
from pathlib import Path


from cutekit import shell, builder, const, project
from cutekit.cmds import Cmd, append
from cutekit.args import Args
from typing import Callable


def kvmAvailable() -> bool:
    if os.path.exists("/dev/kvm") and os.access("/dev/kvm", os.R_OK):
        return True
    return False


class Image:
    logger: logging.Logger
    root: str

    def __init__(self, id: str):
        self.logger = logging.getLogger(f"Image({id})")
        self.root = f"{const.PROJECT_CK_DIR}/images/{id}"

    def withPak(self, id: str, f: Callable[[dict], None]):
        jsonPath = f"{self.root}/bundles/{id}.json"
        pakJson = {"id": id, "objects": {}}
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
            pakJson["objects"][f"bundle://{id}"] = {
                "mime": mime,
                "ref": f"file:/{dest}",
            }

        self.withPak(pak, addToRefs)

        return dest

    def installTo(self, componentSpec: str, targetSpec: str, dest: str):
        self.logger.info(f"Installing {componentSpec} to {dest}...")
        component = builder.build(componentSpec, targetSpec)

        context = component.context

        for depId in component.resolved + [componentSpec]:
            dep = context.componentByName(depId)
            if dep is None:
                raise Exception(f"Component {depId} not found")

            for asset in dep.resfiles():
                self.cpRef(componentSpec, asset[0], f"{depId}/{asset[2]}")

                self.cpRef("_index", asset[0], f"{depId}/{asset[2]}")

        shell.mkdir(Path(f"{self.root}/{dest}").parent)
        shell.cp(component.outfile(), f"{self.root}/{dest}")

    def install(self, componentSpec: str, targetSpec: str):
        self.logger.info(f"Installing {componentSpec}...")
        component = builder.build(componentSpec, targetSpec)
        context = component.context

        for depId in component.resolved + [componentSpec]:
            dep = context.componentByName(depId)
            if dep is None:
                raise Exception(f"Component {depId} not found")

            for asset in dep.resfiles():
                self.cpRef(componentSpec, asset[0], f"{depId}/{asset[2]}")

                self.cpRef("_index", asset[0], f"{depId}/{asset[2]}")

        self.cpRef("_index", component.outfile(), f"{componentSpec}/_bin")
        self.cpRef(componentSpec, component.outfile(), f"{componentSpec}/_bin")

    def cp(self, src: str, dest: str):
        self.logger.info(f"Copying {src} to {dest}...")
        shell.mkdir(Path(f"{self.root}/{dest}").parent)
        shell.cp(src, f"{self.root}/{dest}")

    def cpTree(self, src: str, dest: str):
        self.logger.info(f"Copying {src} to {dest}...")
        shell.mkdir(Path(f"{self.root}/{dest}").parent)
        shell.cpTree(src, f"{self.root}/{dest}")

    def mkdir(self, path: str):
        self.logger.info(f"Creating directory {path}...")
        shell.mkdir(f"{self.root}/{path}")

    def wget(self, url: str, dest: str):
        self.logger.info(f"Downloading {url} to {dest}...")
        shell.mkdir(Path(f"{self.root}/{dest}").parent)
        shell.wget(url, f"{self.root}/{dest}")


class Machine:
    logger: logging.Logger

    def __init__(self, id: str):
        self.logger = logging.getLogger(f"Machine({id})")

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
        self.logger.info("Booting...")

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
            f"file=fat:rw:{image.root},media=disk,format=raw",
            "-display",
            "sdl",
        ]

        if self.logError:
            qemuCmd += ["-d", "int,guest_errors,cpu_reset"]

        if self.useDebug:
            qemuCmd += ["-s", "-S"]

        if kvmAvailable():
            if not self.logError:
                qemuCmd += ["-enable-kvm"]
        else:
            print("KVM not available, using QEMU-TCG")

        shell.exec(*qemuCmd)


def bootCmd(args: Args) -> None:
    project.chdir()

    isDebug = args.consumeOpt("debug", False)
    isErrorLog = args.consumeOpt("dint", False)
    image = Image("efi-x86_64")

    efiTarget = "efi-x86_64" + (":debug" if isDebug else ":o3")
    kernelTarget = "kernel-x86_64" + (":debug" if isDebug else ":o3")
    skiftTarget = "skift-x86_64" + (":debug" if isDebug else ":o3")

    image.mkdir("objects")
    image.mkdir("bundles")

    image.installTo("loader", efiTarget, "EFI/BOOT/BOOTX64.EFI")
    image.install("hjert", kernelTarget)
    image.install("grund-system", skiftTarget)
    image.install("grund-device", skiftTarget)
    image.install("hideo-shell", skiftTarget)
    image.install("skift-branding", skiftTarget)
    image.install("skift-wallpapers", skiftTarget)
    image.install("inter-font", skiftTarget)
    image.install("mdi-font", skiftTarget)

    image.cpTree("meta/image/boot", "boot/")

    machine = QemuSystemAmd64(logError=isErrorLog, useDebug=isDebug)
    machine.boot(image)


append(Cmd("s", "start", "Boot the system", bootCmd))
