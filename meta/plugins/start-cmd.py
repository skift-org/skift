import os

from osdk import utils, shell, builder
from osdk.cmds import Cmd, append
from osdk.args import Args


def kvmAvailable() -> bool:
    if os.path.exists("/dev/kvm") and \
            os.access("/dev/kvm", os.R_OK):
        return True
    return False


def bootCmd(args: Args) -> None:
    imageDir = shell.mkdir(".osdk/images/efi-x86_64")
    efiBootDir = shell.mkdir(f"{imageDir}/EFI/BOOT")
    bootDir = shell.mkdir(f"{imageDir}/boot")

    ovmf = shell.wget(
        "https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd")

    hjert = builder.build("hjert", "kernel-x86_64")
    shell.cp(hjert, f"{bootDir}/kernel.elf")

    loader = builder.build("loader", "efi-x86_64")
    shell.cp(loader, f"{efiBootDir}/BOOTX64.EFI")

    systemSrv = builder.build("system-srv", "skift-x86_64")
    shell.cp(systemSrv, f"{imageDir}/system-srv")

    qemuCmd: list[str] = [
        "qemu-system-x86_64",
        "-machine", "q35",
        "-no-reboot",
        "-no-shutdown",
        # "-d", "guest_errors,cpu_reset,int",
        "-serial", "mon:stdio",
        "-bios", ovmf,
        "-m", "256M",
        "-smp", "4",
        "-drive", f"file=fat:rw:{imageDir},media=disk,format=raw",
    ]

    if kvmAvailable():
        qemuCmd += ["-enable-kvm"]
    else:
        print("KVM not available, using QEMU-TCG")

    shell.exec(*qemuCmd)


append(Cmd("s", "start", "Boot the system", bootCmd))
