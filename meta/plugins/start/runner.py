import os
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
