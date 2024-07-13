#include <abi-sysv/abi.h>
#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry.h>

#include "system.h"

using namespace Grund::System;

Res<> entryPoint(Sys::Context &ctx) {
    try$(Hj::Task::self().label("grund-system"));

    auto system = try$(System::create(ctx));

    try$(system.load("grund-audio"s));
    try$(system.load("grund-conf"s));
    try$(system.load("grund-device"s));
    try$(system.load("grund-dhcp"s));
    try$(system.load("grund-dns"s));
    try$(system.load("grund-echo"s));
    try$(system.load("grund-fs"s));
    try$(system.load("grund-net"s));
    try$(system.load("grund-seat"s));
    try$(system.load("grund-shell"s));

    return system.run();
}

// NOTE: We can't use the normal entryPointAsync because
//       the kernel invoke us with a different signature.
Async::Task<> entryPointAsync(Sys::Context &) {
    unreachable();
}

void __panicHandler(Karm::PanicKind kind, char const *msg);

extern "C" void __entryPoint(usize rawHandover) {
    Abi::SysV::init();
    Karm::registerPanicHandler(__panicHandler);

    auto &ctx = Sys::globalContext();
    char const *argv[] = {"grund-system", nullptr};
    ctx.add<Sys::ArgsHook>(1, argv);
    ctx.add<HandoverHook>((Handover::Payload *)rawHandover);

    auto res = entryPoint(ctx);

    auto self = Hj::Task::self();

    if (not res) {
        logError("{}: {}", argv[0], res.none().msg());
        self.crash().unwrap();
    }

    Abi::SysV::fini();
    self.ret().unwrap();
    unreachable();
}
