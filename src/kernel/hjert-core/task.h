#pragma once

#include <karm-base/func.h>

#include "ctx.h"
#include "object.h"

namespace Hjert::Core {

struct Space;
struct Domain;
struct Ctx;

using Blocker = Func<TimeStamp()>;

enum State {
    RUNNABLE,
    BLOCKED,
    EXITED,
};

struct Task :
    public BaseObject<Task, Hj::Type::TASK> {

    Mode _mode;
    Stack _stack;
    Opt<Box<Ctx>> _ctx;

    Opt<Strong<Space>> _space;
    Opt<Strong<Domain>> _domain;
    Opt<Blocker> _block;

    Flags<Hj::Pledge> _pledges = Hj::Pledge::ALL;

    TimeStamp _sliceEnd = 0;

    static Res<Strong<Task>> create(
        Mode mode,
        Opt<Strong<Space>> space = NONE,
        Opt<Strong<Domain>> domain = NONE);

    static Task &self();

    Task(Mode mode,
         Stack stack,
         Opt<Strong<Space>> space,
         Opt<Strong<Domain>> domain);

    Stack &stack() { return _stack; }

    Space &space() { return *_space.unwrap(); }

    Domain &domain() { return *_domain.unwrap(); }

    bool _ret() const {
        return _signals.has(Hj::Sigs::EXITED) and
               _mode == Mode::USER;
    }

    Res<> ensure(Hj::Pledge pledge);

    Res<> pledge(Hj::Pledge pledge);

    Hj::Pledge pledges() {
        ObjectLockScope scope(*this);
        return _pledges;
    }

    Res<> ready(usize ip, usize sp, Hj::Args args);

    Res<> block(Blocker blocker);

    void crash();

    State eval(TimeStamp now);

    void end(TimeStamp now);

    void save(Arch::Frame const &frame);

    void load(Arch::Frame &frame);

    void enter(Mode mode);

    void leave();
};

} // namespace Hjert::Core
