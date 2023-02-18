#pragma once

#include <karm-base/res.h>
#include <karm-cli/args.h>

using CliArgs = Karm::Cli::Args;

Res<> entryPoint(CliArgs args);
