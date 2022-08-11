#pragma once

#include <karm-base/result.h>
#include <karm-cli/args.h>

using CliResult = Karm::Cli::Result;
using CliArgs = Karm::Cli::Args;

CliResult entryPoint(CliArgs args);
