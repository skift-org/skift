#include <abi-sysv/abi.h>
#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry.h>

#include "bus.h"

using namespace Grund::System;

Res<> entryPoint(Sys::Ctx &ctx) {
    try$(Hj::Task::self().label("grund-system"));

    auto bus = try$(Bus::create());

    auto deviceUnit = try$(Unit::load(ctx, "bundle://grund-device/_bin"_url));
    try$(bus.attach(deviceUnit));

    auto shellUnit = try$(Unit::load(ctx, "bundle://hideo-shell.main/_bin"_url));
    try$(bus.attach(shellUnit));

    return bus.run();
}

// NOTE: We can't use the normal entryPointAsync because
//       the kernel invoke us with a different signature.
Async::Task<> entryPointAsync(Sys::Ctx &) {
    unreachable();
}

void __panicHandler(Karm::PanicKind kind, char const *msg);

extern "C" void __entryPoint(usize rawHandover) {
    Abi::SysV::init();
    Karm::registerPanicHandler(__panicHandler);

    auto &ctx = Sys::globalCtx();
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
