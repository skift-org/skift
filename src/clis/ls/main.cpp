#include <karm-cli/args.h>
#include <karm-sys/dir.h>
#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto allFlag = Cli::flag('a', "all"s, "Do not ignore entries starting with ."s);
    auto listFlag = Cli::flag('l', "list"s, "Use a long listing format."s);

    Cli::Command cmd{
        "ls"s,
        NONE,
        "List directory contents."s,
        {allFlag}
    };

    co_trya$(cmd.execAsync(ctx));

    if (not cmd)
        co_return Ok();

    auto url = co_try$(Mime::parseUrlOrPath("."));
    auto dir = co_try$(Sys::Dir::open(url));

    for (auto const &entry : dir.entries()) {
        if (!allFlag && entry.name[0] == '.')
            continue;

        if (listFlag) {
            auto fileUrl = url / entry.name;
            auto stat = co_try$(Sys::stat(fileUrl));
            Sys::println("{} {} {}", stat.type == Sys::Type::DIR ? "d"s : "-"s, stat.size, stat.modifyTime, entry.name);
        } else {
            Sys::println("{}", entry.name);
        }
    }

    co_return Ok();
}
