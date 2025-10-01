import Karm.Core;

#include "irq.h"

namespace Hjert::Core {

static Vec<Irq*> _irqs;
static Lock _irqsLock;

Res<Arc<Irq>> Irq::create(usize irq) {
    return Ok(makeArc<Irq>(irq));
}

void Irq::trigger(usize irqNum) {
    LockScope _{_irqsLock};
    for (auto* irq : _irqs) {
        if (irq->_irq == irqNum) {
            irq->signal(Hj::Sigs::TRIGGERED, Hj::Sigs::NONE);
        }
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

} // namespace Hjert::Core
