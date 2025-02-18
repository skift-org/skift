#include <karm-logger/logger.h>

#include "channel.h"
#include "domain.h"

namespace Hjert::Core {

Res<Arc<Channel>> Channel::create(usize bufCap, usize capsCap) {
    return Ok(makeArc<Channel>(bufCap, capsCap));
}

Channel::Channel(usize bufCap, usize capsCap)
    : _sr(max(bufCap / 16, 16uz)),
      _bytes(bufCap),
      _caps(capsCap) {
    _updateSignalsUnlock();
}

Res<> Channel::_ensureOpen() {
    if (_closed)
        return Error::brokenPipe("channel closed");
    return Ok();
}

void Channel::_updateSignalsUnlock() {
    _signalUnlock(
        (_sr.len() > 0 ? Hj::Sigs::READABLE : Hj::Sigs::NONE) |
            (_sr.rem() > 0 ? Hj::Sigs::WRITABLE : Hj::Sigs::NONE) |
            (_closed ? Hj::Sigs::CLOSED : Hj::Sigs::NONE),

        (_sr.len() > 0 ? Hj::Sigs::NONE : Hj::Sigs::READABLE) |
            (_sr.rem() > 0 ? Hj::Sigs::NONE : Hj::Sigs::WRITABLE)
    );
}

Res<Hj::SentRecv> Channel::send(Domain& dom, Bytes bytes, Slice<Hj::Cap> caps) {
    ObjectLockScope scope{*this};
    try$(_ensureOpen());

    // Make sure everything is ready for the message
    if (_sr.rem() < 1)
        return Error::invalidInput("not enough space for message");

    if (_bytes.rem() < bytes.len())
        return Error::invalidInput("not enough space for bytes");

    if (_caps.rem() < caps.len())
        return Error::invalidInput("not enough space for caps");

    // Everything is ready, let's send the message
    auto save = _caps.len();
    for (auto cap : caps) {
        auto res = dom.get(cap);
        if (not res) {
            // Uh oh, we need to rollback
            _caps.trunc(save);
            return res.none();
        }
        _caps.pushBack(res.unwrap());
    }

    for (usize i = 0; i < bytes.len(); i++)
        _bytes.pushBack(bytes[i]);

    _sr.pushBack({bytes.len(), caps.len()});

    _updateSignalsUnlock();
    return Ok<Hj::SentRecv>(bytes.len(), caps.len());
}

Res<Hj::SentRecv> Channel::recv(Domain& dom, MutBytes bytes, MutSlice<Hj::Cap> caps) {
    ObjectLockScope scope{*this};
    try$(_ensureOpen());

    ObjectLockScope domScope{dom};

    // Make sure everything is ready for the message
    if (_sr.len() == 0)
        return Error::wouldBlock("no messages available");

    auto& [expectedBytes, expectedCaps] = _sr.peek(0);
    if (bytes.len() < expectedBytes)
        return Error::invalidInput("not enough space for bytes");

    if (caps.len() < expectedCaps)
        return Error::invalidInput("not enough space for caps");

    if (dom._availableUnlocked() < expectedCaps)
        return Error::invalidInput("not enough space in domain");

    // Everything is ready, let's receive the message
    _sr.popFront();

    for (usize i = 0; i < expectedBytes; i++)
        bytes[i] = _bytes.popFront();

    for (usize i = 0; i < expectedCaps; i++)
        // NOTE: We unwrap here because we know that the domain has enough space
        caps[i] = dom.add(Hj::ROOT, _caps.popFront()).unwrap("domain full");

    _updateSignalsUnlock();
    return Ok<Hj::SentRecv>(expectedBytes, expectedCaps);
}

Res<> Channel::close() {
    ObjectLockScope scope{*this};
    _closed = true;
    return Ok();
}

} // namespace Hjert::Core
