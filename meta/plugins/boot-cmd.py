from osdk import utils, build
import shutil
import os

BOOTAGENT = "loader"


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
        "https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd")
    hjert = build.buildOne("kernel-x86_64", "hjert")
    shutil.copy(hjert, f"{bootDir}/kernel.elf")

    if BOOTAGENT == "loader":
        loader = build.buildOne("efi-x86_64", "loader")
        shutil.copy(loader, f"{efiBootDir}/BOOTX64.EFI")
    elif BOOTAGENT == "limine":
        limine = utils.downloadFile(
            "https://github.com/limine-bootloader/limine/raw/v3.0-branch-binary/BOOTX64.EFI")
        limineSys = utils.downloadFile(
            "https://github.com/limine-bootloader/limine/raw/v3.0-branch-binary/limine.sys")
        shutil.copy(limineSys, f"{bootDir}/limine.sys")
        shutil.copy('meta/images/limine-x86_64/limine.cfg',
                    f"{bootDir}/limine.cfg")
        shutil.copy(limine, f"{efiBootDir}/BOOTX64.EFI")

    qemuCmd = [
        "qemu-system-x86_64",
        "-no-reboot",
        "-d", "guest_errors",
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
