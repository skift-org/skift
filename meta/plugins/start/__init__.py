from http import server
from pathlib import Path
from . import image, store, runner
from cutekit import cli, model, shell, const


def generateBoot(img: image.Image) -> None:
    img.mkdir("EFI")
    img.mkdir("EFI/BOOT")

    img.installTo("opstart", "efi-x86_64", "EFI/BOOT/BOOTX64.EFI")
    img.install("hjert", "kernel-x86_64")

    img.cpTree("meta/image/boot/efi", "")


def generateInit(img: image.Image):
    img.mkdir("bundles")
    img.mkdir("share")
    img.mkdir("users/root")

    userspace = [
        "hideo-about.main",
        "hideo-calculator.main",
        "hideo-chat.main",
        "hideo-clock.main",
        "hideo-console.main",
        "hideo-counter.main",
        "hideo-files.main",
        "hideo-settings.main",
        "hideo-sysmon.main",
        "hideo-text.main",
        "hideo-zoo.main",
        "vaev-browser.main",
        "strata-cm",
        "strata-device",
        "strata-fs",
        "strata-input",
        "strata-shell",
    ]

    img.install(userspace, "skift-x86_64")


def generateSystem(registry: model.Registry, hdd: bool = False):
    bootStore = (
        store.Dir("image-efi-x86_64")
        if not hdd
        else store.RawHdd("image-efi-x86_64", 256)
    )
    bootImg = image.Image(registry, bootStore)
    generateBoot(bootImg)

    initStore = store.BootFs("image-init-x86_64")
    initImg = image.Image(registry, initStore)
    generateInit(initImg)

    bootImg.cp(initStore.finalize(), "skift/init.bootfs")

    return bootImg.finalize()


@cli.command("image", "Generate the boot image")
def _() -> None:
    pass


class BuildArgs(model.RegistryArgs):
    debug: bool = cli.arg(None, "debug", "Build the image in debug mode")
    fmt: str = cli.arg(None, "format", "The format of the image")
    compress: str = cli.arg(None, "compress", "Compress the image")
    dist: bool = cli.arg(None, "dist", "Copy the image to the dist folder")


@cli.command("dist", "Generate the boot image")
def _(args: BuildArgs) -> None:
    args.mixins.append("release" if not args.debug else "debug")

    registry = model.Registry.use(args)

    file = generateSystem(registry, args.fmt == "hdd")
    if args.fmt == "hdd" and args.compress:
        zip = shell.compress(file, format=args.compress)
        shell.rmrf(file)
        file = zip

    if args.dist:
        distDir = Path(const.PROJECT_CK_DIR) / "dist"
        shell.mkdir(distDir)
        distPath = distDir / Path(file).name
        shell.cp(file, distPath)
        file = str(distPath)

    print(file)


class StartArgs:
    debug: bool = cli.arg(None, "debug", "Build the image in debug mode")
    arch: str = cli.arg(
        None,
        "arch",
        "The architecture of the image (x86_64 or riscv32)",
        default="x86_64",
    )
    dint: bool = cli.arg(None, "dint", "Debug interrupts")
    clean: bool = cli.arg(None, "clean", "Clean the image before starting")


@cli.command("boot", "Boot the system")
def _(args: StartArgs) -> None:
    if args.arch not in ["x86_64", "riscv32"]:
        raise RuntimeError("Unknown arch")
    rargs = cli.defaults(model.RegistryArgs)
    rargs.mixins.append("release" if not args.debug else "debug")

    registry = model.Registry.use(rargs)

    dirStore = store.Dir(f"image-efi-{args.arch}")
    if args.clean:
        dirStore.clean()

    img = image.Image(registry, dirStore)

    if args.arch == "x86_64":
        generateSystem(registry, False)
        print("Booting...")
        machine = runner.Qemu(logError=args.dint, debugger=args.debug)
        machine.boot(img)
    elif args.arch == "riscv32":
        kernel = img.install("hjert-riscv", "kernel-riscv32")
        qemu = [
            "qemu-system-riscv32",
            "-machine",
            "virt",
            "-bios",
            "default",
            "-nographic",
            "-serial",
            "mon:stdio",
            "--no-reboot",
            "-kernel",
            kernel.path,
        ]
        shell.exec(*qemu)
