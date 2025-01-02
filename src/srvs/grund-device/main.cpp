#include <hjert-api/api.h>
#include <karm-app/inputs.h>
#include <karm-base/map.h>
#include <karm-logger/logger.h>
#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

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

struct RootBus : public Node {
    Rpc::Endpoint &rpc;

    RootBus(Rpc::Endpoint &rpc)
        : rpc{rpc} {}

    Res<> init() override {
        try$(attach(makeStrong<IsaRootBus>()));
        return Ok();
    }

    Res<> bubble(App::Event &e) override {
        if (auto me = e.is<App::MouseEvent>()) {
            try$(rpc.send<App::MouseEvent>(Rpc::Port::BROADCAST, *me));
            e.accept();
        } else if (auto ke = e.is<App::KeyboardEvent>()) {
            try$(rpc.send<App::KeyboardEvent>(Rpc::Port::BROADCAST, *ke));
            e.accept();
        }

        return Node::bubble(e);
    }
};

} // namespace Grund::Device

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);

    logInfo("devices: building device tree...");
    auto root = makeStrong<Grund::Device::RootBus>(endpoint);
    co_try$(root->init());

    logInfo("devices: binding IRQs...");
    auto listener = co_try$(Hj::Listener::create(Hj::ROOT));

    Map<Hj::Cap, usize> cap2irq = {};
    Vec<Hj::Irq> irqs = {};

    for (usize i = 0; i < 16; i++) {
        auto irq = co_try$(Hj::Irq::create(Hj::ROOT, i));
        co_try$(listener.listen(irq, Hj::Sigs::TRIGGERED, Hj::Sigs::NONE));
        cap2irq.put(irq.cap(), i);
        irqs.pushBack(std::move(irq));
    }

    logInfo("service started");

    while (true) {
        co_try$(listener.poll(TimeStamp::endOfTime()));

        while (auto ev = listener.next()) {
            co_try$(Hj::_signal(ev->cap, Hj::Sigs::NONE, Hj::Sigs::TRIGGERED));

            auto irq = cap2irq.tryGet(ev->cap);
            if (irq) {
                auto e = App::makeEvent<Grund::Device::IrqEvent>(*irq);
                co_try$(root->event(*e));
            }
        }
    }

    co_return Ok();
}
