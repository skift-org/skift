from osdk import utils, build
import shutil
import os


def kvmAvailable() -> bool:
    if os.path.exists("/dev/kvm") and \
            os.access("/dev/kvm", os.R_OK):
        return True
    return False


def bootCmd(opts: dict, args: list[str]) -> None:
    imageDir = utils.mkdirP(".osdk/images/efi-x86_64/")
    efiBootDir = utils.mkdirP(".osdk/images/efi-x86_64/EFI/BOOT")
    bootDir = utils.mkdirP(".osdk/images/efi-x86_64/boot")

    ovmf = utils.downloadFile(
        "https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd")

    hjert = build.buildOne("kernel-x86_64", "hjert")
    shutil.copy(hjert, f"{bootDir}/kernel.elf")

    loader = build.buildOne("efi-x86_64", "loader")
    shutil.copy(loader, f"{efiBootDir}/BOOTX64.EFI")

    qemuCmd = [
        "qemu-system-x86_64",
        "-machine", "q35",
        "-no-reboot",
        "-no-shutdown",
        "-d", "guest_errors,cpu_reset,int",
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

    utils.runCmd(*qemuCmd)


__plugin__ = {
    "name": "boot",
    "desc": "Boot a component in a QEMU instance",
    "func": bootCmd,
}
