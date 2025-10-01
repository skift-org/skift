#pragma once

#include "object.h"

namespace Hjert::Core {

struct Irq : BaseObject<Irq, Hj::Type::IRQ> {
    usize _irq;

    static Res<Arc<Irq>> create(usize irq);

    static void trigger(usize irq);

    Irq(usize irq);

    ~Irq() override;
};

} // namespace Hjert::Core
