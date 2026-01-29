#pragma once

#include "object.h"
#include "pipe.h"

namespace Hjert::Core {

struct Irq : BaseObject<Irq, Hj::Type::IRQ> {
    usize _irq;
    bool _eoi = true;
    Opt<Arc<Pipe>> _pipe;

    static Res<Arc<Irq>> create(usize irq);

    static void dispatch(usize irq);

    Irq(usize irq);

    ~Irq() override;

    void trigger();

    void bind(Arc<Pipe> pipe);

    void eoi();
};

} // namespace Hjert::Core
