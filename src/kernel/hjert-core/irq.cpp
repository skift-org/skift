import Karm.Core;

#include "irq.h"

namespace Hjert::Core {

static Vec<Irq*> _irqs;
static Lock _irqsLock;

Res<Arc<Irq>> Irq::create(usize irq) {
    return Ok(makeArc<Irq>(irq));
}

void Irq::dispatch(usize irqNum) {
    LockScope _{_irqsLock};
    for (auto* irq : _irqs) {
        if (irq->_irq == irqNum)
            irq->trigger();
    }
}

Irq::Irq(usize irq) : _irq(irq) {
    LockScope _{_irqsLock};
    _irqs.pushBack(this);
}

Irq::~Irq() {
    LockScope _{_irqsLock};
    _irqs.removeAll(this);
}

void Irq::trigger() {
    ObjectLockScope _{*this};
    _signalUnlock(Hj::Sigs::TRIGGERED, Hj::Sigs::NONE);
    if (_eoi and _pipe) {
        if ((*_pipe)->write({reinterpret_cast<u8*>(&_irq), sizeof(u64)}))
            _eoi = false;
    }
}

void Irq::bind(Arc<Pipe> pipe) {
    ObjectLockScope _{*this};
    _pipe = pipe;
}

void Irq::eoi() {
    ObjectLockScope _{*this};
    _eoi = true;
}

} // namespace Hjert::Core
