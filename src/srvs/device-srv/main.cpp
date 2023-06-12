#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>

#include "cmos.h"
#include "ps2.h"

namespace Dev {

struct IsaRootBus : public Node {
    Res<> init() override {
        auto i18042port = try$(PortIo::open({0x60, 0x8}));
        try$(attach(makeStrong<Ps2::I8042>(i18042port)));

        auto cmosPort = try$(PortIo::open({0x70, 0x2}));
        try$(attach(makeStrong<Cmos::Cmos>(cmosPort)));

        return Ok();
    }
};

} // namespace Dev

Res<> entryPoint(Ctx &) {
    logInfo("devices: building device tree...");
    auto root = makeStrong<Dev::IsaRootBus>();
    try$(root->init());

    logInfo("devices: binding IRQs...");
    auto listener = try$(Hj::Listener::create(Hj::ROOT));
    Vec<Cons<usize, Hj::Irq>> irqs = {};
    for (usize i = 0; i < 16; ++i) {
        irqs.pushBack({i, try$(Hj::Irq::create(Hj::ROOT, i))});
        try$(listener.listen(irqs[i].cdr, Hj::Sigs::TRIGGERED, Hj::Sigs::NONE));
    }

    while (try$(listener.poll(try$(Hj::now()) + TimeSpan::fromSecs(1)))) {
        logInfo("devices: timeout");
    }

    return Ok();
}
