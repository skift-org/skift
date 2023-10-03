#include <hjert-api/api.h>
#include <karm-base/map.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>

#include "cmos.h"
#include "io.h"
#include "ps2.h"

namespace Grund::Device {

struct IsaRootBus : public Node {
    Res<> init() override {
        auto i18042port = try$(PortIo::open({0x60, 0x8}));
        try$(attach(makeStrong<Ps2::I8042>(i18042port)));

        auto cmosPort = try$(PortIo::open({0x70, 0x2}));
        try$(attach(makeStrong<Cmos::Cmos>(cmosPort)));

        return Ok();
    }
};

} // namespace Grund::Device

Res<> entryPoint(Ctx &) {
    logInfo("devices: building device tree...");
    auto root = makeStrong<Grund::Device::IsaRootBus>();
    try$(root->init());

    logInfo("devices: binding IRQs...");
    auto listener = try$(Hj::Listener::create(Hj::ROOT));

    Map<Hj::Cap, usize> cap2irq = {};
    Vec<Hj::Irq> irqs = {};

    for (usize i = 0; i < 16; i++) {
        auto irq = try$(Hj::Irq::create(Hj::ROOT, i));
        try$(listener.listen(irq, Hj::Sigs::TRIGGERED, Hj::Sigs::NONE));
        cap2irq.put(irq.cap(), i);
        irqs.pushBack(std::move(irq));
    }

    while (true) {
        try$(listener.poll(TimeStamp::endOfTime()));

        auto ev = listener.next();
        while (ev) {
            try$(Hj::_signal(ev->cap, Hj::Sigs::NONE, Hj::Sigs::TRIGGERED));

            auto irq = cap2irq.get(ev->cap);
            if (irq) {
                auto e = Async::makeEvent<Grund::Device::IrqEvent>(Async::Propagation::UP, *irq);
                try$(root->event(*e));
            }
            ev = listener.next();
        }
    }

    return Ok();
}
