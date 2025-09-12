#include <karm-sys/dir.h>
#include <karm-sys/entry.h>
#include <karm-sys/proc.h>

import Karm.Cli;

using namespace Karm;

namespace Ls {

struct Options {
    bool all = false;
    bool list = false;
};

Res<> ls(Ref::Url url, Options const& options) {
    auto dir = try$(Sys::Dir::open(url));

    for (auto const& entry : dir.entries()) {
        if (not options.all and entry.name[0] == '.')
            continue;

        if (options.list) {
            auto fileUrl = url / entry.name;
            auto stat = try$(Sys::stat(fileUrl));
            Sys::println("{} {} {} {}", stat.type == Sys::Type::DIR ? "d"s : "-"s, stat.size, stat.modifyTime, entry.name);
        } else {
            Sys::println("{}", entry.name);
        }
    }

    return Ok();
}

Res<> ls(Slice<Str> paths, Options const& options) {
    for (auto& path : paths) {
        if (paths.len() > 1)
            Sys::println("{}:", path);
        auto url = Ref::parseUrlOrPath(path, try$(Sys::pwd()));
        try$(ls(url, options));
    }

    return Ok();
}

} // namespace Ls

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto allFlag = Cli::flag('a', "all"s, "Do not ignore entries starting with ."s);
    auto listFlag = Cli::flag('l', "list"s, "Use a long listing format."s);
    auto argsOperands = Cli::operand<Vec<Str>>("paths"s, "Directories to list."s);

    Cli::Command cmd{
        "ls"s,
        "List directory contents."s,
        {allFlag, listFlag, argsOperands}
    };

    co_trya$(cmd.execAsync(ctx));

    if (not cmd)
        co_return Ok();

    auto options = Ls::Options{
        .all = allFlag,
        .list = listFlag
    };

    if (argsOperands.unwrap())
        co_try$(Ls::ls(argsOperands.unwrap(), options));
    else
        co_try$(Ls::ls(Ref::parseUrlOrPath(".", co_try$(Sys::pwd())), options));

    co_return Ok();
}
