#include <karm-sys/entry.h>
#include <web-dns/dns.h>

Res<> entryPoint(Sys::Ctx &) {
    auto dns = try$(Web::Dns::Client::connect(Web::Dns::GOOGLE));
    auto addr = try$(dns.resolve("google.com"));
    Sys::println("dns resolved domain 'google.com' to {}", addr);
    return Ok();
}
