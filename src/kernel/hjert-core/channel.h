#pragma once

#include <karm-base/ring.h>

#include "object.h"

namespace Hjert::Core {

struct Domain;

struct Channel :
    public BaseObject<Channel, Hj::Type::CHANNEL> {

    bool _closed = false;

    Ring<Hj::SentRecv> _sr;
    Ring<Byte> _bytes;
    Ring<Arc<Object>> _caps;

    static Res<Arc<Channel>> create(usize bufCap = 4096, usize capsCap = 16);

    Channel(usize bufCap, usize capsCap);

    Res<> _ensureOpen();

    void _updateSignalsUnlock();

    Res<Hj::SentRecv> send(Domain &dom, Bytes bytes, Slice<Hj::Cap> caps);

    Res<Hj::SentRecv> recv(Domain &dom, MutBytes bytes, MutSlice<Hj::Cap> caps);

    Res<> close();
};

} // namespace Hjert::Core
