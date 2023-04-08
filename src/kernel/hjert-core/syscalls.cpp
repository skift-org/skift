
#include <karm-cli/style.h>
#include <karm-logger/logger.h>

#include "arch.h"
#include "io.h"
#include "sched.h"
#include "syscalls.h"

namespace Hjert::Core {

Res<> doLog(Task &self, char const *msg, usize len) {
    (void)self;

    auto styledLabel = Cli::styled(self.label(), Cli::style(Cli::random(self.id())).bold());
    logInfo("{}#{}: {}", styledLabel, self.id(), Str{msg, len});
    return Ok();
}

Res<> doCreateDomain(Task &self, Hj::Cap dest, Hj::Cap *cap, usize len) {
    auto obj = try$(Domain::create(len));
    *cap = try$(self.domain().add(dest, obj));
    return Ok();
}

Res<> doCreateTask(Task &self, Hj::Cap dest, Hj::Cap *cap, Hj::Cap domain, Hj::Cap space) {
    auto dom = try$(self.domain().get<Domain>(domain));
    auto spa = try$(self.domain().get<Space>(space));
    auto obj = try$(Task::create(TaskType::USER, std::move(spa), std::move(dom)));
    *cap = try$(self.domain().add(dest, obj));
    return Ok();
}

Res<> doCreateSpace(Task &self, Hj::Cap dest, Hj::Cap *cap) {
    auto obj = try$(Space::create());
    *cap = try$(self.domain().add(dest, obj));
    return Ok();
}

Res<> doCreateMem(Task &self, Hj::Cap dest, Hj::Cap *cap, usize phys, usize len, Hj::MemFlags flags) {
    bool isDma = (flags & Hj::MemFlags::DMA) == Hj::MemFlags::DMA;
    auto obj = try$(isDma
                        ? VNode::makeDma({phys, len})
                        : VNode::alloc(len, flags));
    *cap = try$(self.domain().add(dest, obj));
    return Ok();
}

Res<> doCreateIo(Task &self, Hj::Cap dest, Hj::Cap *cap, usize base, usize len) {
    auto obj = try$(IoNode::create({base, len}));
    *cap = try$(self.domain().add(dest, obj));
    return Ok();
}

Res<> doLabel(Task &self, Hj::Cap cap, char const *label, usize len) {
    auto obj = try$(self.domain().get(cap));
    obj->label({label, len});
    return Ok();
}

Res<> doDrop(Task &self, Hj::Cap cap) {
    return self.domain().drop(cap);
}

Res<> doDup(Task &self, Hj::Cap node, Hj::Cap *dst, Hj::Cap src) {
    auto obj = try$(self.domain().get(src));
    *dst = try$(self.domain().add(node, obj));
    return Ok();
}

Res<> doStart(Task &self, Hj::Cap cap, usize ip, usize sp, Hj::Args const *args) {
    (void)args;
    auto obj = try$(self.domain().get<Task>(cap));
    try$(Sched::instance().start(obj, ip, sp));
    return Ok();
}

Res<> doWait(Task &self, Hj::Cap cap, Hj::Arg *ret) {
    auto obj = try$(self.domain().get<Task>(cap));
    *ret = try$(self.wait(obj));
    return Ok();
}

Res<> doRet(Task &self, Hj::Cap cap, Hj::Arg ret) {
    if (cap.isSelf()) {
        self.ret(ret);
    } else {
        auto task = try$(self.domain().get<Task>(cap));
        task->ret(ret);
    }
    
    return Ok();
}

Res<> doMap(Task &self, Hj::Cap cap, usize *virt, Hj::Cap mem, usize off, usize len, Hj::MapFlags flags) {
    auto memObj = try$(self.domain().get<VNode>(mem));
    auto spaceObj = try$(self.domain().get<Space>(cap));
    *virt = try$(spaceObj->map({*virt, len}, memObj, off, flags)).start;
    return Ok();
}

Res<> doUnmap(Task &self, Hj::Cap cap, usize virt, usize len) {
    auto spaceObj = try$(self.domain().get<Space>(cap));
    try$(spaceObj->unmap({virt, len}));
    return Ok();
}

Res<> doIn(Task &self, Hj::Cap cap, Hj::IoLen len, usize port, Hj::Arg *val) {
    auto obj = try$(self.domain().get<IoNode>(cap));
    *val = try$(obj->in(port, Hj::ioLen2Bytes(len)));
    return Ok();
}

Res<> doOut(Task &self, Hj::Cap cap, Hj::IoLen len, usize port, Hj::Arg val) {
    auto obj = try$(self.domain().get<IoNode>(cap));
    return obj->out(port, Hj::ioLen2Bytes(len), val);
}

Res<> doIpc(Task &self, Hj::Cap *cap, Hj::Cap dst, Hj::Msg *msg, Hj::IpcFlags flags) {
    (void)self;
    (void)cap;
    (void)dst;
    (void)msg;
    (void)flags;
    return Error::notImplemented();
}

Res<> dispatchSyscall(Task &self, Hj::Syscall id, Hj::Args args) {
    switch (id) {
    case Hj::Syscall::LOG:
        return doLog(self, (char const *)args[0], args[1]);

    case Hj::Syscall::CREATE_DOMAIN:
        return doCreateDomain(self, Hj::Cap{args[0]}, (Hj::Cap *)args[1], args[2]);

    case Hj::Syscall::CREATE_TASK:
        return doCreateTask(self, Hj::Cap{args[0]}, (Hj::Cap *)args[1], Hj::Cap{args[2]}, Hj::Cap{args[3]});

    case Hj::Syscall::CREATE_SPACE:
        return doCreateSpace(self, Hj::Cap{args[0]}, (Hj::Cap *)args[1]);

    case Hj::Syscall::CREATE_MEM:
        return doCreateMem(self, Hj::Cap{args[0]}, (Hj::Cap *)args[1], args[2], args[3], Hj::MemFlags{args[4]});

    case Hj::Syscall::CREATE_IO:
        return doCreateIo(self, Hj::Cap{args[0]}, (Hj::Cap *)args[1], args[2], args[3]);

    case Hj::Syscall::LABEL:
        return doLabel(self, Hj::Cap{args[0]}, (char const *)args[1], args[2]);

    case Hj::Syscall::DROP:
        return doDrop(self, Hj::Cap{args[0]});

    case Hj::Syscall::DUP:
        return doDup(self, Hj::Cap{args[0]}, (Hj::Cap *)args[1], Hj::Cap{args[2]});

    case Hj::Syscall::START:
        return doStart(self, Hj::Cap{args[0]}, args[1], args[2], (Hj::Args const *)args[3]);

    case Hj::Syscall::WAIT:
        return doWait(self, Hj::Cap{args[0]}, (Hj::Arg *)args[1]);

    case Hj::Syscall::RET:
        return doRet(self, Hj::Cap{args[0]}, args[1]);

    case Hj::Syscall::MAP:
        return doMap(self, Hj::Cap{args[0]}, (usize *)args[1], Hj::Cap{args[2]}, args[3], args[4], (Hj::MapFlags)args[5]);

    case Hj::Syscall::UNMAP:
        return doUnmap(self, Hj::Cap{args[0]}, args[1], args[2]);

    case Hj::Syscall::IN:
        return doIn(self, Hj::Cap{args[0]}, (Hj::IoLen)args[1], args[2], (Hj::Arg *)args[3]);

    case Hj::Syscall::OUT:
        return doOut(self, Hj::Cap{args[0]}, (Hj::IoLen)args[1], args[2], args[3]);

    case Hj::Syscall::IPC:
        return doIpc(self, (Hj::Cap *)args[0], Hj::Cap{args[1]}, (Hj::Msg *)args[2], (Hj::IpcFlags)args[3]);

    default:
        return Error::invalidInput("invalid syscall id");
    }
}

Res<> doSyscall(Hj::Syscall id, Hj::Args args) {

    auto &self = Task::self();
    self.enterSupervisorMode();
    auto res = dispatchSyscall(self, id, args);
    if (not res) {
        logError("Syscall {}({}) failed: {}", Hj::toStr(id), (Hj::Arg)id, res.none().msg());
        for (auto i = 0; i < 6; ++i)
            logDebug("    arg{}: {p} {}", i, (usize)args[i], (isize)args[i]);
    }
    self.leaveSupervisorMode();
    return res;
}

} // namespace Hjert::Core
