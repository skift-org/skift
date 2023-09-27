
#include <karm-cli/style.h>
#include <karm-logger/logger.h>

#include "arch.h"
#include "channel.h"
#include "domain.h"
#include "iop.h"
#include "irq.h"
#include "listener.h"
#include "sched.h"
#include "syscalls.h"
#include "task.h"
#include "user.h"

namespace Hjert::Core {

Res<> doNow(Task &self, User<TimeStamp> ts) {
    return ts.store(self.space(), Sched::instance()._stamp);
}

Res<> doLog(Task &self, UserSlice<char const> msg) {
    try$(self.ensure(Hj::Pledge::LOG));
    return msg.with<Str>(self.space(), [&](Str str) -> Res<> {
        struct LoggerScope {
            LoggerScope() {
                Logger::_Embed::loggerLock();
            }
            ~LoggerScope() {
                Logger::_Embed::loggerUnlock();
            }
        } scope;
        auto styledLabel = Cli::styled(self.label(), Cli::style(Cli::random(self.id())));
        try$(Fmt::format(Hjert::Arch::loggerOut(), "{}({}) ", styledLabel, self.id()));
        try$(Hjert::Arch::loggerOut().writeStr(str));
        return Ok();
    });
}

Res<> doCreate(Task &self, Hj::Cap dest, User<Hj::Cap> out, User<Hj::Props> p) {
    auto props = try$(p.load(self.space()));
    auto obj = try$(props.visit(
        Visitor{
            [&](Hj::DomainProps &) -> Res<Strong<Object>> {
                return Ok(try$(Domain::create()));
            },
            [&](Hj::TaskProps &props) -> Res<Strong<Object>> {
                try$(self.ensure(Hj::Pledge::TASK));

                auto dom = props.domain.isRoot()
                               ? try$(self._domain)
                               : try$(self.domain().get<Domain>(props.domain));

                auto spa = props.space.isRoot()
                               ? try$(self._space)
                               : try$(self.domain().get<Space>(props.space));

                auto obj = try$(Task::create(Mode::USER, spa, dom));

                auto pledges = self.pledges();
                try$(obj->pledge(pledges));

                return Ok(obj);
            },
            [&](Hj::SpaceProps &) -> Res<Strong<Object>> {
                try$(self.ensure(Hj::Pledge::MEM));

                return Ok(try$(Space::create()));
            },
            [&](Hj::VmoProps &props) -> Res<Strong<Object>> {
                try$(self.ensure(Hj::Pledge::MEM));

                bool isDma = (props.flags & Hj::VmoFlags::DMA) == Hj::VmoFlags::DMA;

                if (isDma) {
                    try$(self.ensure(Hj::Pledge::HW));
                    return Ok(try$(Vmo::makeDma({props.phys, props.len})));
                }

                return Ok(try$(Vmo::alloc(props.len, props.flags)));
            },
            [&](Hj::IopProps &props) -> Res<Strong<Object>> {
                try$(self.ensure(Hj::Pledge::HW));
                return Ok(try$(Iop::create({props.base, props.len})));
            },
            [&](Hj::ChannelProps &props) -> Res<Strong<Object>> {
                return Ok(try$(Channel::create(props.cap)));
            },
            [&](Hj::IrqProps &props) -> Res<Strong<Object>> {
                try$(self.ensure(Hj::Pledge::HW));
                return Ok(try$(Irq::create(props.irq)));
            },
            [&](Hj::ListenerProps &) -> Res<Strong<Object>> {
                return Ok(try$(Listener::create()));
            },
        }));

    return out.store(self.space(), try$(self.domain().add(dest, obj)));
}

Res<> doLabel(Task &self, Hj::Cap cap, UserSlice<char const> label) {
    return label.with<Str>(self.space(), [&](Str str) -> Res<> {
        if (cap.isRoot())
            self.label(str);
        else
            try$(self.domain().get(cap))->label(str);

        return Ok();
    });
}

Res<> doDrop(Task &self, Hj::Cap cap) {
    return self.domain().drop(cap);
}

Res<> doPledge(Task &self, Hj::Cap cap, Flags<Hj::Pledge> pledges) {
    if (cap.isRoot()) {
        return self.pledge(pledges);
    }

    auto obj = try$(self.domain().get<Task>(cap));
    return obj->pledge(pledges);
}

Res<> doDup(Task &self, Hj::Cap dest, User<Hj::Cap> out, Hj::Cap cap) {
    if (cap.isRoot())
        return Error::invalidHandle("cannot duplicate root");

    auto obj = try$(self.domain().get(cap));
    return out.store(self.space(), try$(self.domain().add(dest, obj)));
}

Res<> doStart(Task &self, Hj::Cap cap, usize ip, usize sp, User<Hj::Args const> args) {
    (void)args;
    auto obj = try$(self.domain().get<Task>(cap));
    try$(obj->ready(ip, sp, try$(args.load(self.space()))));
    try$(Sched::instance().enqueue(obj));
    return Ok();
}

Res<> doMap(Task &self, Hj::Cap cap, User<usize> virt, Hj::Cap vmo, usize off, User<usize> len, Hj::MapFlags flags) {
    auto vmoObj = try$(self.domain().get<Vmo>(vmo));
    auto spaceObj = cap.isRoot()
                        ? try$(self._space)
                        : try$(self.domain().get<Space>(cap));

    auto vrange = Hal::VmmRange{
        try$(virt.load(self.space())),
        try$(len.load(self.space())),
    };

    vrange = try$(spaceObj->map(vrange, vmoObj, off, flags));

    try$(virt.store(self.space(), vrange.start));
    try$(len.store(self.space(), vrange.size));

    return Ok();
}

Res<> doUnmap(Task &self, Hj::Cap cap, usize virt, usize len) {
    auto spaceObj = cap.isRoot()
                        ? try$(self._space)
                        : try$(self.domain().get<Space>(cap));
    try$(spaceObj->unmap({virt, len}));
    return Ok();
}

Res<> doIn(Task &self, Hj::Cap cap, Hj::IoLen len, usize port, User<Hj::Arg> val) {
    auto obj = try$(self.domain().get<Iop>(cap));
    return val.store(self.space(), try$(obj->in(port, Hj::ioLen2Bytes(len))));
}

Res<> doOut(Task &self, Hj::Cap cap, Hj::IoLen len, usize port, Hj::Arg val) {
    auto obj = try$(self.domain().get<Iop>(cap));
    return obj->out(port, Hj::ioLen2Bytes(len), val);
}

Res<> doSend(Task &self, Hj::Cap cap, User<Hj::Msg> msg, Hj::Cap from) {
    auto obj = try$(self.domain().get<Channel>(cap));
    auto dom = try$(self.domain().get<Domain>(from));
    return obj->send(*dom, try$(msg.load(self.space())));
}

Res<> doRecv(Task &self, Hj::Cap cap, User<Hj::Msg> msg, Hj::Cap to) {
    auto obj = try$(self.domain().get<Channel>(cap));
    auto dom = try$(self.domain().get<Domain>(to));
    return msg.store(self.space(), try$(obj->recv(*dom)));
}

Res<> doClose(Task &self, Hj::Cap cap) {
    auto obj = try$(self.domain().get<Channel>(cap));
    return obj->close();
}

Res<> doSignal(Task &self, Hj::Cap cap, Hj::Sigs set, Hj::Sigs unset) {
    if (cap.isRoot()) {
        self.signal(set, unset);
        return Ok();
    }

    auto obj = try$(self.domain().get(cap));
    obj->signal(set, unset);
    return Ok();
}

Res<> doListen(Task &self, Hj::Cap cap, Hj::Cap target, Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
    auto obj = try$(self.domain().get<Listener>(cap));
    auto targetObj = try$(self.domain().get(target));
    return obj->listen(target, targetObj, set, unset);
}

Res<> doPoll(Task &self, Hj::Cap cap, UserSlice<Hj::Event> events, User<usize> evLen, TimeStamp deadline) {
    auto obj = try$(self.domain().get<Listener>(cap));

    try$(self.block([&]() {
        auto events = obj->poll();
        if (events.len() > 0)
            return TimeStamp::epoch();
        return deadline;
    }));

    ObjectLockScope lock{*obj};
    auto l = min(events.len(), obj->events().len());
    try$(evLen.store(self.space(), l));
    try$(events.with<MutSlice<Hj::Event>>(self.space(), [&](auto events) {
        for (usize i = 0; i < l; ++i) {
            events[i] = obj->events()[i];
        }
        obj->flush();
        return Ok();
    }));

    return Ok();
}

Res<> dispatchSyscall(Task &self, Hj::Syscall id, Hj::Args args) {
    switch (id) {
    case Hj::Syscall::NOW:
        return doNow(self, args[0]);

    case Hj::Syscall::LOG:
        return doLog(self, {args[0], args[1]});

    case Hj::Syscall::CREATE:
        return doCreate(self, Hj::Cap{args[0]}, args[1], args[2]);

    case Hj::Syscall::LABEL:
        return doLabel(self, Hj::Cap{args[0]}, {args[1], args[2]});

    case Hj::Syscall::DROP:
        return doDrop(self, Hj::Cap{args[0]});

    case Hj::Syscall::PLEDGE:
        return doPledge(self, Hj::Cap{args[0]}, (Hj::Pledge)args[1]);

    case Hj::Syscall::DUP:
        return doDup(self, Hj::Cap{args[0]}, args[1], Hj::Cap{args[2]});

    case Hj::Syscall::START:
        return doStart(self, Hj::Cap{args[0]}, args[1], args[2], args[3]);

    case Hj::Syscall::MAP:
        return doMap(self, Hj::Cap{args[0]}, args[1], Hj::Cap{args[2]}, args[3], args[4], (Hj::MapFlags)args[5]);

    case Hj::Syscall::UNMAP:
        return doUnmap(self, Hj::Cap{args[0]}, args[1], args[2]);

    case Hj::Syscall::IN:
        return doIn(self, Hj::Cap{args[0]}, (Hj::IoLen)args[1], args[2], args[3]);

    case Hj::Syscall::OUT:
        return doOut(self, Hj::Cap{args[0]}, (Hj::IoLen)args[1], args[2], args[3]);

    case Hj::Syscall::SEND:
        return doSend(self, Hj::Cap{args[0]}, args[1], Hj::Cap{args[2]});

    case Hj::Syscall::RECV:
        return doRecv(self, Hj::Cap{args[0]}, args[1], Hj::Cap{args[2]});

    case Hj::Syscall::CLOSE:
        return doClose(self, Hj::Cap{args[0]});

    case Hj::Syscall::SIGNAL:
        return doSignal(self, Hj::Cap{args[0]}, (Hj::Sigs)args[1], (Hj::Sigs)args[2]);

    case Hj::Syscall::LISTEN:
        return doListen(self, Hj::Cap{args[0]}, Hj::Cap{args[1]}, (Hj::Sigs)args[2], (Hj::Sigs)args[3]);

    case Hj::Syscall::POLL:
        return doPoll(self, Hj::Cap{args[0]}, {args[1], args[2]}, args[3], args[4]);

    default:
        return Error::invalidInput("invalid syscall id");
    }
}

Res<> doSyscall(Hj::Syscall id, Hj::Args args) {
    auto &self = Task::self();
    self.enter(Mode::SUPER);
    auto res = dispatchSyscall(self, id, args);
    if (not res) {
        logError("Syscall {}({}) failed: {}", Hj::toStr(id), (Hj::Arg)id, res.none().msg());
        for (auto i = 0; i < 6; ++i)
            logDebug("    arg{}: {p} {}", i, (usize)args[i], (isize)args[i]);
    }
    self.leave();
    return res;
}

} // namespace Hjert::Core
