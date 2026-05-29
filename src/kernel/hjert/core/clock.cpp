module;

#include <hal/mem.h>
#include <hal/pmm.h>
#include <karm/macros>

export module Hjert.Core:clock;

import Karm.Core;

import :vmo;
import :object;
import :mem;

using namespace Karm;

namespace Hjert::Core {

export struct Clock;

static Lock _clocksLock;
static Vec<Clock*> _clocks = {};
static Instant _monotonicClock{};

export struct Clock : BaseObject<Clock, Hj::Type::CLOCK> {
    Arc<Vmo> _vmo;
    Atomic<_TimeVal>* _reg = nullptr;

    static Res<Arc<Clock>> create() {
        auto vmo = try$(Vmo::alloc(Hal::PAGE_SIZE, Hal::PmmFlags::LOWER));
        auto addr = try$(kmm().pmm2Kmm(vmo->range())).start;
        auto reg = reinterpret_cast<Atomic<Karm::_TimeVal>*>(addr);
        return Ok(makeArc<Clock>(vmo, reg));
    }

    static Instant monotonicInstant() {
        LockScope _{_clocksLock};
        return _monotonicClock;
    }

    static void monotonicTick(Duration span) {
        LockScope _{_clocksLock};
        _monotonicClock += span;
        for (auto* clock : _clocks)
            clock->tick(span);
    }

    Clock(Arc<Vmo> vmo, Atomic<_TimeVal>* reg)
        : _vmo(vmo), _reg(reg) {
        LockScope _{_clocksLock};
        _clocks.pushBack(this);
    }

    ~Clock() {
        LockScope _{_clocksLock};
        _clocks.removeAll(this);
    }

    void tick(Duration span) {
        _reg->fetchAdd(span.val());
    }
};

} // namespace Hjert::Core
