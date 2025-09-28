
#include <abi-sysv/abi.h>
#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry.h>
#include "fd.h"

void __panicHandler(Karm::PanicKind kind, char const* msg) {
    Hj::log(msg).unwrap();

    if (kind == Karm::PanicKind::PANIC) {
        Hj::Task::self().crash().unwrap();
        __builtin_unreachable();
    }
}

extern "C" [[gnu::weak]] void __entryPoint(usize rawHandover, usize rawIn, usize rawOut) {
    Abi::SysV::init();
    Karm::registerPanicHandler(__panicHandler);

    Sys::Context ctx;
    char const* argv[] = {"service", nullptr};
    ctx.add<Sys::ArgsHook>(1, argv);
    ctx.add<HandoverHook>(reinterpret_cast<Handover::Payload*>(rawHandover));
    auto fd = makeRc<Skift::IpcFd>(Hj::Cap{rawIn}, Hj::Cap{rawOut});
    ctx.add<Sys::ChannelHook>(Sys::IpcConnection{fd, ""_url});

    auto res = Sys::run(entryPointAsync(ctx));

    auto self = Hj::Task::self();

    if (not res) {
        logError("{}: {}", argv[0], res.none().msg());
        self.crash().unwrap();
    }

    Abi::SysV::fini();
    self.ret().unwrap();
    unreachable();
}
