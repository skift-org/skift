#include <karm-pkg>
#include <karm-sys/entry.h>
#include <karm-sys>

Async::Task<> entryPointAsync(Sys::Context &) {
    auto currentBundle = co_try$(Pkg::currentBundle());
    Sys::println("Current bundle:");
    Sys::println("\tid: {}", currentBundle.id);
    Sys::println("\turl: {}", currentBundle.url());

    Sys::println();

    auto bundles = co_try$(Pkg::installedBundles());
    Sys::println("Bundles:");
    for (auto bundle : bundles) {
        Sys::println("\tBundle {#}", bundle.id);
        Sys::println("\turl: {}", bundle.url());
        Sys::println();
    }

    co_return Ok();
}
