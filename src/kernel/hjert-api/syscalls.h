#pragma once

#include "types.h"

namespace Hj {

Res<> _syscall(Syscall syscall, Arg a0 = 0, Arg a1 = 0, Arg a2 = 0, Arg a3 = 0, Arg a4 = 0, Arg a5 = 0);

Res<> _log(char const *msg, usize len);

Res<> _createDomain(Cap dest, Cap *cap);

Res<> _createTask(Cap dest, Cap *cap, Cap node, Cap space);

Res<> _createSpace(Cap dest, Cap *cap);

Res<> _createVmo(Cap dest, Cap *cap, usize phys, usize len, VmoFlags flags = VmoFlags::NONE);

Res<> _createIo(Cap dest, Cap *cap, usize base, usize len);

Res<> _createChannel(Cap dest, Cap *cap, usize len);

Res<> _createIrq(Cap dest, Cap *cap, usize irq);

Res<> _label(Cap cap, char const *label, usize len);

Res<> _drop(Cap cap);

Res<> _dup(Cap node, Cap *dst, Cap src);

Res<> _start(Cap cap, usize ip, usize sp, Args const *args);

Res<> _map(Cap cap, usize *virt, Cap vmo, usize off, usize len, MapFlags flags = MapFlags::NONE);

Res<> _unmap(Cap cap, usize virt, usize len);

Res<> _in(Cap cap, IoLen len, usize port, Arg *val);

Res<> _out(Cap cap, IoLen len, usize port, Arg val);

Res<> _send(Cap cap, Msg const *msg, Cap from);

Res<> _recv(Cap cap, Msg *msg, Cap to);

Res<> _close(Cap cap);

Res<> _signal(Cap cap, Signals set, Signals unset, Arg val);

Res<> _wait(Cond const *conds, usize condLen, Event *ev, usize *evLen, Karm::TimeStamp deadline);

} // namespace Hj
