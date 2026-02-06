import Abi.SysV;
import Karm.Sys.Skift;
import Karm.Logger;
import Hjert.Api;

#include <karm/entry>
#include <vaerk-handover/hook.h>

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

    Sys::IpcConnection conn = {
        makeRc<Sys::Skift::DuplexFd>(Hj::Cap{rawIn}, Hj::Cap{rawOut}),
        "ipc:strata-cm"_url,
    };
    Sys::Skift::setupClient(std::move(conn));

    Async::Cancellation cancellation;
    auto res = Sys::run(entryPointAsync(ctx, cancellation.token()));

    auto self = Hj::Task::self();

    if (not res) {
        logError("{}: {}", argv[0], res);
        self.crash().unwrap();
    }

    Abi::SysV::fini();
    self.ret().unwrap();
    unreachable();
}
