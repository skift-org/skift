#include <karm-net/dns/dns.h>
#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto& args = Sys::useArgs(ctx);
    if (args.len() != 1) {
        Sys::println("usage: {} <domain>", args.self());
        co_return Error::invalidInput("invalid number of arguments");
    }

    auto dns = co_try$(Net::Dns::Client::connect(Net::Dns::GOOGLE));
    auto addr = co_try$(dns.resolve(args[0]));
    Sys::println("dns resolved domain '{}' to {}", args[0], addr);
    co_return Ok();
}
