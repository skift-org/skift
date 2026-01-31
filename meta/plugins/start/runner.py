import os
import subprocess
import logging

from cutekit import shell
from .image import Image


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


def hvfAvailable() -> bool:
    return "hvf" in str(
        subprocess.check_output(["qemu-system-x86_64", "-accel", "help"])
    )


def sdlAvailable() -> bool:
    return "sdl" in str(
        subprocess.check_output(["qemu-system-x86_64", "-display", "help"])
    )


class Qemu(Machine):
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
            "-chardev",
            "stdio,id=char0,signal=on",
            "-serial",
            "chardev:char0",
            "-bios",
            ovmf,
            "-m",
            "512M",
            "-smp",
            "4",
            "-drive",
            f"file=fat:rw:{image.finalize()},media=disk,format=raw",
        ]


        if sdlAvailable():
            qemuCmd += ["-display", "sdl"]
        if self.logError:
            qemuCmd += ["-d", "int,guest_errors,cpu_reset"]

        if self.debugger:
            qemuCmd += ["-s", "-S"]

        if not self.logError:
            if kvmAvailable():
                qemuCmd += ["-enable-kvm"]
            elif hvfAvailable():
                qemuCmd += ["-accel", "hvf"]
            else:
                qemuCmd += ["-accel", "tcg"]

        shell.exec(*qemuCmd)
