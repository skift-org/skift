#include "objects.h"

#include "arch.h"
#include "mem.h"
#include "sched.h"

namespace Hjert::Core {

/* --- Object --------------------------------------------------------------- */

Atomic<usize> Object::_counter = 0;

void Object::label(Str label) {
    LockScope scope(_lock);
    _label = String(label);
}

Str Object::label() const {
    return _label ? *_label : "<no label>";
}

void Object::_signalUnlock(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
    _signals |= set;
    _signals &= ~unset;
}

Flags<Hj::Sigs> Object::_pollUnlock() {
    return _signals;
}

void Object::signal(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
    LockScope scope(_lock);
    _signalUnlock(set, unset);
}

Flags<Hj::Sigs> Object::poll() {
    LockScope scope(_lock);
    return _pollUnlock();
}

/* --- Domain --------------------------------------------------------------- */

Res<Strong<Domain>> Domain::create() {
    return Ok(makeStrong<Domain>());
}

Res<Hj::Cap> Domain::add(Hj::Cap dest, Strong<Object> obj) {
    ObjectLockScope scope(*this);

    auto c = dest.raw();

    if (c != 0)
        return try$(get<Domain>(c & MASK))->add(c >> SHIFT, obj);

    for (c = 1; c < _slots.len(); c++) {
        if (not _slots[c]) {
            _slots[c] = obj;
            return Ok(c);
        }
    }

    return Error::invalidHandle("no free slots");
}

Res<Strong<Object>> Domain::get(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    auto c = cap.raw();

    if (c & ~MASK)
        return try$(get<Domain>(c & MASK))->get(c >> SHIFT);

    if (not _slots[c])
        return Error::invalidHandle("slot is empty");

    return Ok(*_slots[c]);
}

Res<> Domain::drop(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    auto c = cap.raw();

    if (c & ~MASK)
        return try$(get<Domain>(c & MASK))->drop(c >> SHIFT);

    if (not _slots[c])
        return Error::invalidHandle("slot is empty");

    _slots[c] = NONE;
    return Ok();
}

/* --- Vmo ------------------------------------------------------------------ */

Res<Strong<VNode>> VNode::alloc(usize size, Hj::VmoFlags) {
    if (size == 0) {
        return Error::invalidInput("size is zero");
    }

    try$(ensureAlign(size, Hal::PAGE_SIZE));
    Hal::PmmMem mem = try$(pmm().allocOwned(size, Hal::PmmFlags::UPPER));
    return Ok(makeStrong<VNode>(std::move(mem)));
}

Res<Strong<VNode>> VNode::makeDma(Hal::DmaRange prange) {
    if (prange.size == 0) {
        return Error::invalidInput("size is zero");
    }

    try$(prange.ensureAligned(Hal::PAGE_SIZE));
    return Ok(makeStrong<VNode>(prange));
}

Hal::PmmRange VNode::range() {
    return _mem.visit(
        Visitor{
            [](Hal::PmmMem const &mem) {
                return mem.range();
            },
            [](Hal::DmaRange const &range) {
                return range.as<Hal::PmmRange>();
            },
        });
}

/* --- Space ---------------------------------------------------------------- */

Res<Strong<Space>> Space::create() {
    return Ok(makeStrong<Space>(try$(Arch::createVmm())));
}

Space::Space(Strong<Hal::Vmm> vmm) : _vmm(vmm) {
    _alloc.unused({0x400000, 0x800000000000});
}

Space::~Space() {
    while (_maps.len()) {
        unmap(_maps.peekFront().vrange)
            .unwrap("unmap failed");
    }
}

Res<usize> Space::_lookup(Hal::VmmRange vrange) {
    for (usize i = 0; i < _maps.len(); i++) {
        auto &map = _maps[i];
        if (Op::eq(map.vrange, vrange)) {
            return Ok(i);
        }
    }

    return Error::invalidInput("no such mapping");
}

Res<Hal::VmmRange> Space::map(Hal::VmmRange vrange, Strong<VNode> vmo, usize off, Hj::MapFlags flags) {
    ObjectLockScope scope(*this);

    try$(vrange.ensureAligned(Hal::PAGE_SIZE));

    if (vrange.size == 0) {
        vrange.size = vmo->range().size;
    }

    auto end = try$(checkedAdd(off, vrange.size));

    if (end > vmo->range().size) {
        return Error::invalidInput("mapping too large");
    }

    if (vrange.start == 0) {
        vrange = try$(_alloc.alloc(vrange.size));
    } else {
        _alloc.used(vrange);
    }

    auto map = Map{vrange, off, std::move(vmo)};

    try$(vmm().mapRange(map.vrange, {map.vmo->range().start + map.off, vrange.size}, flags | Hal::VmmFlags::USER));
    try$(vmm().flush(map.vrange));

    _maps.pushBack(std::move(map));

    return Ok(vrange);
}

Res<> Space::unmap(Hal::VmmRange vrange) {
    ObjectLockScope scope(*this);

    try$(vrange.ensureAligned(Hal::PAGE_SIZE));

    auto id = try$(_lookup(vrange));
    auto &map = _maps[id];

    try$(vmm().free(map.vrange));
    try$(vmm().flush(map.vrange));

    _alloc.unused(map.vrange);
    _maps.removeAt(id);
    return Ok();
}

void Space::activate() {
    vmm().activate();
}

/* --- Channel -------------------------------------------------------------- */

Res<Parcel> Parcel::fromMsg(Domain &dom, Hj::Msg msg) {
    Parcel parcel;
    parcel._label = msg.label;
    parcel._flags = msg.flags;

    for (usize i = 0; i < 6; i++) {
        if (msg.isCap(i)) {
            parcel._slots[i] = try$(dom.get(try$(msg.loadCap(i))));
        } else {
            parcel._slots[i] = try$(msg.loadVal(i));
        }
    }

    return Ok(Parcel{});
}

Res<Hj::Msg> Parcel::toMsg(Domain &dom) {
    Hj::Msg msg{_label};

    for (usize i = 0; i < 6; i++) {
        try$(_slots[i].visit(
            Visitor{
                [&](None) -> Res<> {
                    return Ok();
                },
                [&](Hj::Arg arg) -> Res<> {
                    msg.store(i, arg);
                    return Ok();
                },
                [&](Strong<Object> obj) -> Res<> {
                    msg.store(i, try$(dom.add(Hj::ROOT, obj)));
                    return Ok();
                },
            }));
    }

    return Ok(msg);
}

Res<Strong<Channel>> Channel::create(usize cap) {
    return Ok(makeStrong<Channel>(cap));
}

Channel::Channel(usize cap) {
    _cap = cap;
}

Res<> Channel::_ensureNoEmpty() {
    if (_ring.len() == 0) {
        return Error::wouldBlock("channel empty");
    }
    return Ok();
}

Res<> Channel::_ensureNoFull() {
    if (_ring.len() == _cap) {
        return Error::wouldBlock("channel full");
    }
    return Ok();
}

Res<> Channel::_ensureOpen() {
    if (_closed) {
        return Error::brokenPipe("channel closed");
    }
    return Ok();
}

void Channel::_updateSignalsUnlock() {
    _signalUnlock(
        _closed ? Hj::Sigs::CLOSED : Hj::Sigs::NONE,
        _closed ? Hj::Sigs::NONE : Hj::Sigs::CLOSED);
}

Res<> Channel::send(Domain &dom, Hj::Msg msg) {
    ObjectLockScope scope{*this};
    try$(_ensureOpen());
    try$(_ensureNoFull());
    _ring.pushBack(try$(Parcel::fromMsg(dom, msg)));
    return Ok();
}

Res<Hj::Msg> Channel::recv(Domain &dom) {
    ObjectLockScope scope{*this};
    try$(_ensureOpen());
    try$(_ensureNoEmpty());
    auto parcel = _ring.popBack();
    return parcel.toMsg(dom);
}

Res<> Channel::close() {
    ObjectLockScope scope{*this};
    _closed = true;
    return Ok();
}

/* --- Listener ------------------------------------------------------------- */

Res<Strong<Listener>> Listener::create() {
    return Ok(makeStrong<Listener>());
}

Res<> Listener::watch(Hj::Cap cap, Strong<Object> obj, Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
    ObjectLockScope scope{*this};

    for (usize i = 0; i < _listened.len(); ++i) {
        if (_listened[i].cap == cap) {
            auto &listened = _listened[i];
            listened.set = set;
            listened.unset = unset;

            if (listened.set.empty() and
                listened.unset.empty()) {
                _listened.removeAt(i);
                logInfo("listener: stopped listening to cap {}", cap.raw());
                return Ok();
            }
        }
    }

    _listened.pushBack(Listened{cap, obj, set, unset});
    logInfo("listener: started listening to cap {}", cap.raw());
    return Ok();
}

Slice<Hj::Event> Listener::listen() {
    ObjectLockScope scope{*this};
    _events.clear();

    for (auto &l : _listened) {
        auto sigs = l.obj->poll();
        if (sigs & l.set) {
            _events.pushBack(Hj::Event{l.cap, sigs & l.set, true});
        }

        if (~sigs & l.unset) {
            _events.pushBack(Hj::Event{l.cap, sigs & l.unset, false});
        }
    }

    return _events;
}

/* --- Task ----------------------------------------------------------------- */

Res<Strong<Task>> Task::create(
    TaskMode mode,
    Opt<Strong<Space>> space,
    Opt<Strong<Domain>> domain) {

    logInfo("task: creating task...");
    auto stack = try$(Stack::create());
    auto ctx = try$(Ctx::create(stack.loadSp()));
    auto task = makeStrong<Task>(mode, std::move(stack), std::move(ctx), space, domain);
    return Ok(task);
}

Task &Task::self() {
    return *Sched::instance()._curr;
}

Res<> Task::block(Blocker blocker) {
    // NOTE: Can't use ObjectLockScope here because we need to yield
    //       outside of the lock.
    _lock.acquire();
    _block = std::move(blocker);
    _lock.release();
    Sched::instance().yield();
    return Ok();
}

bool Task::unblock(TimeStamp now) {
    ObjectLockScope scope(*this);

    if (_block) {
        if (Op::gt((*_block)(), now)) {
            return false;
        }
        _block = NONE;
    }

    return false;
}

void Task::crash() {
    logError("task: crashed");
    signal(Hj::Sigs::EXITED | Hj::Sigs::CRASHED,
           Hj::Sigs::NONE);
}

void Task::enterSupervisorMode() {
    ObjectLockScope scope(*this);
    _mode = TaskMode::SUPER;
}

void Task::leaveSupervisorMode() {
    // NOTE: Can't use ObjectLockScope here because we need to yield
    //       outside of the lock.
    _lock.acquire();
    _mode = TaskMode::USER;
    bool shouldYield = _hasRetUnlock();
    _lock.release();

    if (shouldYield)
        Sched::instance().yield();
}

void Task::enterIdleMode() {
    ObjectLockScope scope(*this);
    _mode = TaskMode::IDLE;
}

/* --- I/O ------------------------------------------------------------------ */

Res<Strong<IoNode>> IoNode::create(Hal::PortRange range) {
    return Ok(makeStrong<IoNode>(range));
}

IoNode::IoNode(Hal::PortRange range)
    : _range(range) {}

Res<Hj::Arg> IoNode::in(usize offset, usize size) {
    return Ok(Hal::Io::port(_range).readSized(offset, size));
}

Res<> IoNode::out(usize offset, usize size, Hj::Arg arg) {
    Hal::Io::port(_range).writeSized(offset, size, arg);
    return Ok();
}

/* --- IRQ ------------------------------------------------------------------ */

static Vec<Irq *> _irqs;
static Lock _irqsLock;

Res<Strong<Irq>> Irq::create(usize irq) {
    return Ok(makeStrong<Irq>(irq));
}

void Irq::trigger(usize irqNum) {
    LockScope scope{_irqsLock};
    for (auto *irq : _irqs) {
        if (irq->_irq == irqNum) {
            irq->signal(Hj::Sigs::TRIGGERED, Hj::Sigs::NONE);
        }
    }
}

Irq::Irq(usize irq) : _irq(irq) {
    LockScope scope{_irqsLock};
    _irqs.pushBack(this);
}

Irq::~Irq() {
    LockScope scope{_irqsLock};
    _irqs.removeAll(this);
}

} // namespace Hjert::Core
