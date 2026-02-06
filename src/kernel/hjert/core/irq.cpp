export module Hjert.Core:irq;

import :object;
import :pipe;

namespace Hjert::Core {

struct Irq;

static Vec<Irq*> _irqs;
static Lock _irqsLock;

struct Irq : BaseObject<Irq, Hj::Type::IRQ> {
    usize _irq;
    bool _eoi = true;
    Opt<Arc<Pipe>> _pipe;

    static Res<Arc<Irq>> create(usize irq) {
        return Ok(makeArc<Irq>(irq));
    }

    static void dispatch(usize irqNum) {
        LockScope _{_irqsLock};
        for (auto* irq : _irqs) {
            if (irq->_irq == irqNum)
                irq->trigger();
        }
    }

    Irq(usize irq) : _irq(irq) {
        LockScope _{_irqsLock};
        _irqs.pushBack(this);
    }

    ~Irq() override {
        LockScope _{_irqsLock};
        _irqs.removeAll(this);
    }

    void trigger() {
        ObjectLockScope _{*this};
        _signalUnlock(Hj::Sigs::TRIGGERED, Hj::Sigs::NONE);
        if (_eoi and _pipe) {
            if ((*_pipe)->write({reinterpret_cast<u8*>(&_irq), sizeof(u64)}))
                _eoi = false;
        }
    }

    void bind(Arc<Pipe> pipe) {
        ObjectLockScope _{*this};
        _pipe = pipe;
    }

    void eoi() {
        ObjectLockScope _{*this};
        _eoi = true;
    }
};

} // namespace Hjert::Core
