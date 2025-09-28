#include <abi-sysv/abi.h>
#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry.h>

#include "bus.h"

using namespace Strata::Bus;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_try$(Hj::Task::self().label("strata-bus"));

    logInfo("skiftOS " stringify$(__ck_version_value));

    auto system = co_try$(Bus::create(ctx));

    co_try$(system->attach(makeRc<System>()));
    co_try$(system->prepareService("strata-device"s));
    co_try$(system->prepareService("strata-shell"s));

    for (auto& endpoint : system->_endpoints)
        co_try$(endpoint->activate(ctx));

    co_return Sys::globalSched().wait(Instant::endOfTime());
}

void __panicHandler(Karm::PanicKind kind, char const* msg);

extern "C" void __entryPoint(usize rawHandover) {
    Abi::SysV::init();
    Karm::registerPanicHandler(__panicHandler);

    Sys::Context ctx;
    char const* argv[] = {"strata-bus", nullptr};
    ctx.add<Sys::ArgsHook>(1, argv);
    ctx.add<HandoverHook>((Handover::Payload*)rawHandover);

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
