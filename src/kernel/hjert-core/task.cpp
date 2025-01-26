#include <karm-logger/logger.h>

#include "arch.h"
#include "sched.h"
#include "space.h"
#include "task.h"

namespace Hjert::Core {

Res<Arc<Task>> Task::create(
    Mode mode,
    Opt<Arc<Space>> space,
    Opt<Arc<Domain>> domain
) {
    auto stack = try$(Stack::create());
    auto task = makeArc<Task>(mode, std::move(stack), space, domain);
    return Ok(task);
}

Task &Task::self() {
    return *globalSched()._curr;
}

Task::Task(
    Mode mode,
    Stack stack,
    Opt<Arc<Space>> space,
    Opt<Arc<Domain>> domain
)
    : _mode(mode),
      _stack(std::move(stack)),
      _space(space),
      _domain(domain) {
}

Res<> Task::ensure(Hj::Pledge pledge) {
    ObjectLockScope scope(*this);

    if (not _pledges.has(pledge)) {
        return Error::permissionDenied("task does not have pledge");
    }
    return Ok();
}

Res<> Task::pledge(Hj::Pledge pledge) {
    ObjectLockScope scope(*this);
    if (not _pledges.has(pledge))
        return Error::permissionDenied("task does not have pledge");
    _pledges = pledge;
    return Ok();
}

Res<> Task::ready(usize ip, usize sp, Hj::Args args) {
    ObjectLockScope scope(*this);
    _ctx = try$(Arch::createContext(_mode, ip, sp, stack().loadSp(), args));
    return Ok();
}

Res<> Task::block(Blocker blocker) {
    // NOTE: If the blocker is already expired, don't block.
    if (blocker() <= globalSched()._stamp)
        return Ok();

    // NOTE: Can't use ObjectLockScope here because
    //       we need to yield outside of the lock.
    _lock.acquire();
    _block = std::move(blocker);
    _lock.release();
    Arch::yield();
    return Ok();
}

void Task::crash() {
    logError("{}: crashed", *this);
    signal(
        Hj::Sigs::EXITED | Hj::Sigs::CRASHED,
        Hj::Sigs::NONE
    );
}

State Task::eval(Instant now) {
    ObjectLockScope scope(*this);

    if (_ret())
        return State::EXITED;

    if (_block) {
        if ((*_block)() > now) {
            return State::BLOCKED;
        }
        _block = NONE;
    }

    return State::RUNNABLE;
}

void Task::save(Arch::Frame const &frame) {
    (*_ctx)->save(frame);
}

void Task::load(Arch::Frame &frame) {
    if (_space)
        (*_space)->activate();

    (*_ctx)->load(frame);
}

void Task::enter(Mode mode) {
    ObjectLockScope scope(*this);
    _mode = mode;
}

void Task::leave() {
    // NOTE: Can't use ObjectLockScope here because
    //       we need to yield outside of the lock.
    _lock.acquire();
    _mode = Mode::USER;
    bool yield = _ret();
    _lock.release();

    if (yield)
        Arch::yield();
}

} // namespace Hjert::Core
