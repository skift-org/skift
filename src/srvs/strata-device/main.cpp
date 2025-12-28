#include <hjert-api/api.h>
#include <karm-sys/entry.h>

import Karm.Core;
import Karm.App;
import Strata.Device;
import Karm.Logger;

namespace Strata::Device {

struct IsaRootBus : Node {
    Res<> init() override {
        auto i18042port = try$(PortIo::open({0x60, 0x8}));
        try$(attach(makeRc<Ps2::I8042>(i18042port)));

        auto cmosPort = try$(PortIo::open({0x70, 0x2}));
        try$(attach(makeRc<Cmos::Cmos>(cmosPort)));

        return Ok();
    }
};

struct RootBus : Node {
    Sys::Endpoint& rpc;

    RootBus(Sys::Endpoint& rpc)
        : rpc{rpc} {}

    Res<> init() override {
        try$(attach(makeRc<IsaRootBus>()));
        return Ok();
    }

    Res<> bubble(App::Event& e) override {
        if (auto me = e.is<App::MouseEvent>()) {
            try$(rpc.send<App::MouseEvent>(Sys::Port::BROADCAST, *me));
            e.accept();
        } else if (auto ke = e.is<App::KeyboardEvent>()) {
            try$(rpc.send<App::KeyboardEvent>(Sys::Port::BROADCAST, *ke));
            e.accept();
        }

        return Node::bubble(e);
    }
};

} // namespace Strata::Device

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken) {
    auto endpoint = Sys::Endpoint::adopt(ctx);

    logInfo("devices: building device tree...");
    auto root = makeRc<Strata::Device::RootBus>(endpoint);
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
        co_try$(listener.poll(Instant::endOfTime()));

        while (auto ev = listener.next()) {
            co_try$(Hj::_signal(ev->cap, Hj::Sigs::NONE, Hj::Sigs::TRIGGERED));

            auto irq = cap2irq.tryGet(ev->cap);
            if (irq) {
                auto e = App::makeEvent<Strata::Device::IrqEvent>(*irq);
                co_try$(root->event(*e));
            }
        }
    }

    co_return Ok();
}
