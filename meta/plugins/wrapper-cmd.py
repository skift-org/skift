from cutekit import cli, ensure

ensure((0, 6, 0))


@cli.command(None, "nuke-tools", "Nuke the development tools")
@cli.command(None, "setup", "Setup the development environment")
def nop(args: cli.Args):
    raise RuntimeError("Don't use ck directly, use ./skift.sh instead.")
