#include <abi-sysv/abi.h>
#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-main/base.h>
#include <karm-panic/panic.h>

#include "hooks.h"

void __panicHandler(Karm::PanicKind kind, char const *msg) {
    Hj::log(msg).unwrap();

    if (kind == Karm::PanicKind::PANIC) {
        Hj::Task::self().crash().unwrap();
        __builtin_unreachable();
    }
}

extern "C" void __entryPoint(usize rawHandover, usize rawIn, usize rawOut) {
    Abi::SysV::init();
    Karm::registerPanicHandler(__panicHandler);

    Ctx ctx;
    char const *argv[] = {"service", nullptr};
    ctx.add<ArgsHook>(1, argv);
    ctx.add<HandoverHook>((Handover::Payload *)rawHandover);
    ctx.add<ChannelsHook>(Hj::Cap{rawIn}, Hj::Cap{rawOut});

    auto res = entryPoint(ctx);

    auto self = Hj::Task::self();

    if (not res) {
        logError("{}: {}", argv[0], res.none().msg());
        (void)self.crash();
    }

    Abi::SysV::fini();
    (void)self.ret();
    asm volatile("ud2");
}
