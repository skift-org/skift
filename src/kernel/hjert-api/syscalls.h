#pragma once

#include "types.h"

namespace Hj {

Res<> _now(TimeStamp *ts);

Res<> _log(char const *msg, usize len);

Res<> _create(Cap dest, Cap *out, Props const *props);

Res<> _label(Cap cap, char const *label, usize len);

Res<> _drop(Cap cap);

Res<> _pledge(Cap cap, Flags<Pledge> pledges);

Res<> _dup(Cap dest, Cap *out, Cap cap);

Res<> _start(Cap cap, usize ip, usize sp, Args const *args);

Res<> _map(Cap cap, usize *virt, Cap vmo, usize off, usize *len, MapFlags flags = MapFlags::NONE);

Res<> _unmap(Cap cap, usize virt, usize len);

Res<> _in(Cap cap, IoLen len, usize port, Arg *val);

Res<> _out(Cap cap, IoLen len, usize port, Arg val);

Res<> _send(Cap cap, Byte const *buf, usize bufLen, Cap const *caps, usize capLen);

Res<> _recv(Cap cap, Byte *buf, usize *bufLen, Cap *caps, usize *capLen);

Res<> _close(Cap cap);

Res<> _signal(Cap cap, Flags<Sigs> set, Flags<Sigs> unset);

Res<> _listen(Cap cap, Cap target, Flags<Sigs> set, Flags<Sigs> unset);

Res<> _poll(Cap cap, Event *ev, usize evCap, usize *evLen, TimeStamp until);

} // namespace Hj
