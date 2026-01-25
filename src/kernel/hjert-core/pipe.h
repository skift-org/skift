#pragma once

import Karm.Core;

#include "object.h"

namespace Hjert::Core {

struct Pipe : BaseObject<Pipe, Hj::Type::PIPE> {
    bool _closed;
    Ring<u8> _bytes;

    static Res<Arc<Pipe>> create(usize bufCap = 4096);

    Pipe(usize bufCap);

    Res<> _ensureOpen();

    void _updateSignalsUnlock();

    Res<usize> write(Bytes in);

    Res<usize> read(MutBytes out);

    Res<> close();
};

} // namespace Hjert::Core