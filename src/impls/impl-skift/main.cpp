#include <abi-sysv/abi.h>
#include <handover/spec.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-main/base.h>

extern "C" void __entryPoint(usize ho) {
    Abi::SysV::init();

    Handover::Payload *payload = (Handover::Payload *)ho;
    logInfo("handover agent: '{}'", payload->agentName());

    auto res = entryPoint({});

    auto self = Hj::Task::self();

    if (not res) {
        (void)self.ret(-(isize)res.none().code());
    }

    Abi::SysV::fini();
    (void)self.ret();

    while (true)
        asm volatile("pause");
}
