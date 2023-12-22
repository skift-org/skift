from . import image, store, runner
from cutekit import cli, model, shell


def generateSystem(img: image.Image) -> None:
    img.mkdir("objects")
    img.mkdir("bundles")

    img.mkdir("EFI")
    img.mkdir("EFI/BOOT")
    img.installTo("loader", "efi-x86_64", "EFI/BOOT/BOOTX64.EFI")

    img.install("hjert", "kernel-x86_64")

    img.install("grund-system", "skift-x86_64")
    img.install("grund-device", "skift-x86_64")
    img.install("hideo-shell", "skift-x86_64")
    img.install("skift-branding", "skift-x86_64")
    img.install("skift-wallpapers", "skift-x86_64")
    img.install("inter-font", "skift-x86_64")
    img.install("mdi-font", "skift-x86_64")

    img.cpTree("meta/image/boot", "boot")


@cli.command(None, "image", "Generate the boot image")
def _(args: cli.Args) -> None:
    pass


@cli.command("g", "image/build", "Generate the boot image")
def _(args: cli.Args) -> None:
    debug = args.consumeOpt("debug", False) is True
    fmt = args.consumeOpt("format", "dir")
    compress = args.consumeOpt("compress", "zstd") is True
    args.opts["mixins"] = "debug" if debug else "o3"

    registry = model.Registry.use(args)

    s = (
        store.Dir("image-efi-x86_64")
        if fmt == "dir"
        else store.RawHdd("image-efi-x86_64", 256)
    )
    img = image.Image(registry, s)
    generateSystem(img)
    file = img.finalize()
    if fmt == "hdd":
        if compress:
            zip = shell.compress(file, format=compress)
            shell.rmrf(file)
            file = zip
    print(file)


@cli.command("s", "image/start", "Boot the system")
def _(args: cli.Args) -> None:
    debug = args.consumeOpt("debug", False) is True
    args.opts["mixins"] = "debug" if debug else "o3"

    registry = model.Registry.use(args)

    logError = args.consumeOpt("dint", False) is True

    img = image.Image(registry, store.Dir("image-efi-x86_64"))
    generateSystem(img)
    machine = runner.Qemu(logError=logError, debugger=debug)
    machine.boot(img)
