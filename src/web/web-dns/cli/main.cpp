#include <karm-sys/entry.h>
#include <web-dns/dns.h>

Res<> entryPoint(Sys::Ctx &ctx) {
    auto &args = Sys::useArgs(ctx);
    if (args.len() != 1) {
        Sys::println("usage: {} <domain>", args.self());
        return Error::invalidInput("invalid number of arguments");
    }
    auto dns = try$(Web::Dns::Client::connect(Web::Dns::GOOGLE));
    auto addr = try$(dns.resolve(args[0]));
    Sys::println("dns resolved domain '{}' to {}", args[0], addr);
    return Ok();
}
