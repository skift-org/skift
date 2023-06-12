#include <hjert-api/api.h>
#include <karm-base/map.h>
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

    Map<Hj::Cap, usize> cap2irq = {};
    Vec<Hj::Irq> irqs = {};

    for (usize i = 0; i < 16; i++) {
        auto irq = try$(Hj::Irq::create(Hj::ROOT, i));
        try$(listener.listen(irq, Hj::Sigs::TRIGGERED, Hj::Sigs::NONE));
        logInfo("devices: bound IRQ {} to {}", i, irq._cap.raw());
        cap2irq.put(irq._cap, i);
        irqs.pushBack(std::move(irq));
    }

    try$(listener.poll(try$(Hj::now()) + TimeSpan::fromSecs(1)));
    while (true) {
        auto ev = listener.next();
        while (ev) {
            logInfo("devices: got an event from {}", ev->cap.raw());
            try$(Hj::_signal(ev->cap, Hj::Sigs::NONE, Hj::Sigs::TRIGGERED));

            auto irq = cap2irq.get(ev->cap);
            if (not irq) {
                logWarn("devices: got an event from an unknown IRQ cap {}", ev->cap.raw());
            } else {
                logInfo("devices: irq {} triggered", *irq);
            }
            ev = listener.next();
        }
        try$(listener.poll(try$(Hj::now()) + TimeSpan::fromSecs(1)));
    }

    return Ok();
}
