#include "pipe.h"

namespace Hjert::Core {

Res<Arc<Pipe>> Pipe::create(usize bufCap) {
    return Ok(makeArc<Pipe>(bufCap));
}

Pipe::Pipe(usize bufCap)
    : _bytes(bufCap) {
    _updateSignalsUnlock();
}

Res<> Pipe::_ensureOpen() {
    if (_closed)
        return Error::brokenPipe("channel closed");
    return Ok();
}

void Pipe::_updateSignalsUnlock() {
    _signalUnlock(
        {
            (_bytes.len() > 0 ? Hj::Sigs::READABLE : Hj::Sigs::NONE),
            (_bytes.rem() > 0 ? Hj::Sigs::WRITABLE : Hj::Sigs::NONE),
            (_closed ? Hj::Sigs::CLOSED : Hj::Sigs::NONE),
        },
        {
            (_bytes.len() > 0 ? Hj::Sigs::NONE : Hj::Sigs::READABLE),
            (_bytes.rem() > 0 ? Hj::Sigs::NONE : Hj::Sigs::WRITABLE),
        }
    );
}

Res<usize> Pipe::write(Bytes in) {
    ObjectLockScope scope{*this};
    try$(_ensureOpen());

    if (_bytes.rem() < 1)
        return Error::wouldBlock("not enough space for data");

    auto written = min(_bytes.rem(), in.len());

    for (usize i = 0; i < written; i++)
        _bytes.pushBack(in[i]);

    _updateSignalsUnlock();
    return Ok(written);
}

Res<usize> Pipe::read(MutBytes out) {
    ObjectLockScope scope{*this};

    if (_bytes.len() == 0) {
        try$(_ensureOpen());
        return Error::wouldBlock("no data available");
    }

    auto read = min(_bytes.len(), out.len());
    for (usize i = 0; i < read; i++)
        out[i] = _bytes.popFront();

    _updateSignalsUnlock();

    return Ok(read);
}

Res<> Pipe::close() {
    ObjectLockScope scope{*this};
    _closed = true;
    _updateSignalsUnlock();
    return Ok();
}

} // namespace Hjert::Core
