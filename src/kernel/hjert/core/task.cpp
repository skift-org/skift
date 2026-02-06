module;

#include <karm/macros>

export module Hjert.Core:task;

import Karm.Core;
import :space;
import :domain;
import :context;
import :clock;

namespace Hjert::Core {

export using Blocker = Func<Instant()>;

enum struct State {
    RUNNABLE,
    BLOCKED,
    EXITED,
};

export struct Task : BaseObject<Task, Hj::Type::TASK> {
    Hj::Mode _mode;
    Stack _stack;
    Opt<Box<Context>> _ctx;

    Opt<Arc<Space>> _space;
    Opt<Arc<Domain>> _domain;
    Opt<Blocker> _block;

    Flags<Hj::Pledge> _pledges = Hj::Pledge::ALL;

    Instant _sliceEnd = 0;

    static Res<Arc<Task>> create(
        Hj::Mode mode,
        Opt<Arc<Space>> space = NONE,
        Opt<Arc<Domain>> domain = NONE
    ) {
        auto stack = try$(Stack::create());
        auto task = makeArc<Task>(mode, std::move(stack), space, domain);
        return Ok(task);
    }

    Task(
        Hj::Mode mode,
        Stack stack,
        Opt<Arc<Space>> space,
        Opt<Arc<Domain>> domain
    ) : _mode(mode),
        _stack(std::move(stack)),
        _space(space),
        _domain(domain) {
    }

    Stack& stack() { return _stack; }

    Space& space() { return *_space.unwrap(); }

    Domain& domain() { return *_domain.unwrap(); }

    bool _ret() const {
        return _signals.has(Hj::Sigs::EXITED) and
               _mode == Hj::Mode::USER;
    }

    Res<> ensure(Flags<Hj::Pledge> pledge) {
        ObjectLockScope _(*this);

        if (not _pledges.has(pledge)) {
            return Error::permissionDenied("task does not have pledge");
        }
        return Ok();
    }

    Res<> pledge(Flags<Hj::Pledge> pledge) {
        ObjectLockScope _(*this);
        if (not _pledges.has(pledge))
            return Error::permissionDenied("task does not have pledge");
        _pledges = pledge;
        return Ok();
    }

    Flags<Hj::Pledge> pledges() {
        ObjectLockScope _(*this);
        return _pledges;
    }

    Res<> ready(usize ip, usize sp, Hj::Args args) {
        ObjectLockScope _(*this);
        _ctx = try$(Arch::createContext(_mode, ip, sp, stack().loadSp(), args));
        return Ok();
    }

    Res<> block(Blocker blocker) {
        // NOTE: If the blocker is already expired, don't block.
        if (blocker() <= clockNow())
            return Ok();

        // NOTE: Can't use ObjectLockScope here because
        //       we need to yield outside the lock.
        _lock.acquire();
        _block = std::move(blocker);
        _lock.release();
        Arch::yield();
        return Ok();
    }

    void crash() {
        logError("{}: crashed", *this);
        signal(
            {Hj::Sigs::EXITED, Hj::Sigs::CRASHED},
            Hj::Sigs::NONE
        );
    }

    State eval(Instant now) {
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

    void save(Arch::Frame const& frame) {
        (*_ctx)->save(frame);
    }

    void load(Arch::Frame& frame) {
        if (_space)
            (*_space)->activate();

        (*_ctx)->load(frame);
    }

    void enter(Hj::Mode mode) {
        ObjectLockScope _(*this);
        _mode = mode;
    }

    void leave() {
        // NOTE: Can't use ObjectLockScope here because
        //       we need to yield outside the lock.
        _lock.acquire();
        _mode = Hj::Mode::USER;
        bool yield = _ret();
        _lock.release();

        if (yield)
            Arch::yield();
    }
};

} // namespace Hjert::Core
