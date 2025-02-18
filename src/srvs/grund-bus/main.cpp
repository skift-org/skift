#include <abi-sysv/abi.h>
#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry.h>

#include "bus.h"

using namespace Grund::Bus;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_try$(Hj::Task::self().label("grund-bus"));

    logInfo("skiftOS " stringify$(__ck_version_value));

    auto system = co_try$(Bus::create(ctx));

    co_try$(system->attach(makeRc<System>()));
    co_try$(system->prepareService("grund-av"s));
    co_try$(system->prepareService("grund-conf"s));
    co_try$(system->prepareService("grund-device"s));
    co_try$(system->prepareService("grund-dhcp"s));
    co_try$(system->prepareService("grund-dns"s));
    co_try$(system->prepareService("grund-echo"s));
    co_try$(system->prepareService("grund-fs"s));
    co_try$(system->prepareService("grund-net"s));
    co_try$(system->prepareService("grund-seat"s));
    co_try$(system->prepareService("grund-shell"s));

    for (auto& endpoint : system->_endpoints)
        co_try$(endpoint->activate(ctx));

    co_return Sys::globalSched().wait(Instant::endOfTime());
}

void __panicHandler(Karm::PanicKind kind, char const* msg);

extern "C" void __entryPoint(usize rawHandover) {
    Abi::SysV::init();
    Karm::registerPanicHandler(__panicHandler);

    auto& ctx = Sys::globalContext();
    char const* argv[] = {"grund-bus", nullptr};
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
