#pragma once

import Karm.Core;

#include "context.h"
#include "object.h"

namespace Hjert::Core {

struct Space;
struct Domain;
struct Context;

using Blocker = Func<Instant()>;

enum State {
    RUNNABLE,
    BLOCKED,
    EXITED,
};

struct Task :
    public BaseObject<Task, Hj::Type::TASK> {

    Mode _mode;
    Stack _stack;
    Opt<Box<Context>> _ctx;

    Opt<Arc<Space>> _space;
    Opt<Arc<Domain>> _domain;
    Opt<Blocker> _block;

    Flags<Hj::Pledge> _pledges = Hj::Pledge::ALL;

    Instant _sliceEnd = 0;

    static Res<Arc<Task>> create(
        Mode mode,
        Opt<Arc<Space>> space = NONE,
        Opt<Arc<Domain>> domain = NONE
    );

    static Task& self();

    Task(
        Mode mode,
        Stack stack,
        Opt<Arc<Space>> space,
        Opt<Arc<Domain>> domain
    );

    Stack& stack() { return _stack; }

    Space& space() { return *_space.unwrap(); }

    Domain& domain() { return *_domain.unwrap(); }

    bool _ret() const {
        return _signals.has(Hj::Sigs::EXITED) and
               _mode == Mode::USER;
    }

    Res<> ensure(Flags<Hj::Pledge> pledge);

    Res<> pledge(Flags<Hj::Pledge> pledge);

    Flags<Hj::Pledge> pledges() {
        ObjectLockScope scope(*this);
        return _pledges;
    }

    Res<> ready(usize ip, usize sp, Hj::Args args);

    Res<> block(Blocker blocker);

    void crash();

    State eval(Instant now);

    void end(Instant now);

    void save(Arch::Frame const& frame);

    void load(Arch::Frame& frame);

    void enter(Mode mode);

    void leave();
};

} // namespace Hjert::Core
