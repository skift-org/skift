#include <karm-cli/args.h>
#include <karm-sys/entry.h>
#include <karm-sys/lookup.h>
#include <karm-sys/proc.h>

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto domainArg = Cli::operand<Str>("domain"s, "Domain name to lookup"s);

    Cli::Command cmd{
        "net-lookup"s,
        NONE,
        "Do a domain name lookup"s,
        {
            domainArg,
        }
    };

    co_trya$(cmd.execAsync(ctx));

    auto ips = co_trya$(Sys::lookupAsync(domainArg));
    co_try$(Io::format(Sys::out(), "{}:\n", domainArg.unwrap()));
    for (auto& ip : ips) {
        co_try$(Io::format(Sys::out(), "{}\n", ip));
    }

    co_return Ok();
}
