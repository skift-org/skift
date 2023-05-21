#include <hjert-core/arch.h>
#include <hjert-core/cpu.h>
#include <hjert-core/mem.h>
#include <hjert-core/sched.h>
#include <hjert-core/syscalls.h>
#include <karm-logger/logger.h>
#include <karm-text/witty.h>

#include <hal-x86_64/com.h>
#include <hal-x86_64/cpuid.h>
#include <hal-x86_64/gdt.h>
#include <hal-x86_64/idt.h>
#include <hal-x86_64/pic.h>
#include <hal-x86_64/pit.h>
#include <hal-x86_64/simd.h>
#include <hal-x86_64/sys.h>
#include <hal-x86_64/vmm.h>

#include "ints.h"

namespace Hjert::Arch {

static x86_64::Com _com1 = x86_64::Com::com1();

static x86_64::DualPic _pic = x86_64::DualPic::dualPic();
static x86_64::Pit _pit = x86_64::Pit::pit();

static Array<Byte, Hal::PAGE_SIZE> _kstackRsp{};
static Array<Byte, Hal::PAGE_SIZE> _kstackIst{};
static x86_64::Tss _tss{};

static x86_64::Gdt _gdt{_tss};
static x86_64::GdtDesc _gdtDesc{_gdt};

static x86_64::Idt _idt{};
static x86_64::IdtDesc _idtDesc{_idt};

Res<> init(Handover::Payload &) {

    _com1.init();

    _gdtDesc.load();
    _tss = {};
    _tss._rsp[0] = (u64)_kstackRsp.bytes().end();
    _tss._ist[0] = (u64)_kstackIst.bytes().end();
    x86_64::_tssUpdate();

    for (usize i = 0; i < x86_64::Idt::LEN; i++) {
        _idt.entries[i] = x86_64::IdtEntry{_intVec[i], 0, x86_64::IdtEntry::GATE};
    }

    _idtDesc.load();

    _pic.init();
    _pit.init(1000);

    x86_64::simdInit();
    x86_64::sysInit(_sysHandler);

    return Ok();
}

Io::TextWriter &loggerOut() {
    return _com1;
}

void stopAll() {
    while (true) {
        x86_64::cli();
        x86_64::hlt();
    }
}

/* --- Cpu ------------------------------------------------------------------ */

struct Cpu : public Core::Cpu {
    void enableInterrupts() override {
        x86_64::sti();
    }

    void disableInterrupts() override {
        x86_64::cli();
    }

    void relaxe() override {
        x86_64::hlt();
    }
};

static Cpu _cpu{};

Core::Cpu &cpu() {
    return _cpu;
}

/* --- Interrupts ----------------------------------------------------------- */

static char const *_faultMsg[32] = {
    "division-by-zero",
    "debug",
    "non-maskable-interrupt",
    "breakpoint",
    "detected-overflow",
    "out-of-bounds",
    "invalid-opcode",
    "no-coprocessor",
    "double-fault",
    "coprocessor-segment-overrun",
    "bad-tss",
    "segment-not-present",
    "stack-fault",
    "general-protection-fault",
    "page-fault",
    "unknown-interrupt",
    "coprocessor-fault",
    "alignment-check",
    "machine-check",
    "simd-floating-point-exception",
    "virtualization-exception",
    "control-protection-exception",
    "reserved",
    "hypervisor-injection-exception",
    "vmm-communication-exception",
    "security-exception",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
};

void backtrace(usize rbp) {
    usize *frame = reinterpret_cast<usize *>(rbp);

    while (frame) {
        usize ip = frame[1];
        usize sp = frame[0];

        logPrint("    ip={p}, sp={p}", ip, sp);

        frame = reinterpret_cast<usize *>(sp);
    }
}

auto const *CLOSE_LINE = "-----------------------------------------------------------";

usize switchTask(TimeSpan span, usize sp) {
    Core::Task::self().saveCtx(sp);
    Core::Sched::instance().schedule(span);
    return Core::Task::self().loadCtx();
}

extern "C" usize _intDispatch(usize sp) {
    auto *frame = reinterpret_cast<Frame *>(sp);

    cpu().beginInterrupt();

    if (frame->intNo < 32) {
        if (frame->cs == (x86_64::Gdt::UCODE * 8 | 3)) {
            logPrint("userspace fault:'{}'", _faultMsg[frame->intNo]);
            logPrint("int={} err={} rip={p} rsp={p} cr2={p} cr3={p}", frame->intNo, frame->errNo, frame->rip, frame->rsp, x86_64::rdcr2(), x86_64::rdcr3());
            Core::Task::self().crash();
            sp = switchTask(TimeSpan::fromMSecs(0), sp);
        } else {
            logPrint("{}--- {} {}----------------------------------------------------", Cli::style(Cli::YELLOW_LIGHT), Cli::styled("!!!", Cli::Style(Cli::Color::RED).bold()), Cli::style(Cli::YELLOW_LIGHT));
            logPrint("");
            logPrint("    {}", Cli::styled("Kernel Panic", Cli::style(Cli::RED).bold()));
            logPrint("    CPU EXCEPTION : '{}'", _faultMsg[frame->intNo]);
            logPrint("    {}", Cli::styled(Text::witty(frame->rsp + frame->rip), Cli::GRAY_DARK));
            logPrint("");
            logPrint("    {}", Cli::styled("Registers", Cli::WHITE));
            logPrint("    int={}", frame->intNo);
            logPrint("    err={}", frame->errNo);
            logPrint("    rip={p}", frame->rip);
            logPrint("    rsp={p}", frame->rsp);
            logPrint("    cr2={p}", x86_64::rdcr2());
            logPrint("    cr3={p}", x86_64::rdcr3());
            logPrint("");
            logPrint("    {}", Cli::styled("Backtrace", Cli::WHITE));
            backtrace(frame->rbp);
            logPrint("");
            logPrint("    {}", Cli::styled("System halted", Cli::WHITE));
            logPrint("");
            logPrint("{}", Cli::styled("-----------------------------------------------------------", Cli::YELLOW_LIGHT));
            panic("cpu exception");
        }
    } else if (frame->intNo == 100) {
        sp = switchTask(TimeSpan::fromMSecs(0), sp);
    } else {
        isize irq = frame->intNo - 32;

        if (irq == 0) {
            sp = switchTask(TimeSpan::fromMSecs(1), sp);
        } else {
            logInfo("x86_64: irq: {}", irq);
        }

        Core::Irq::trigger(irq);
        _pic.ack(frame->intNo);
    }

    cpu().endInterrupt();

    return sp;
}

extern "C" usize _sysDispatch(usize sp) {
    auto *frame = reinterpret_cast<Frame *>(sp);
    auto result = Core::doSyscall(
        (Hj::Syscall)frame->rax,
        {frame->rdi, frame->rsi, frame->rdx, frame->r10, frame->r8, frame->r9});

    if (not result) {
        return (usize)result.none().code();
    }
    return (usize)Error::_OK;
}

static x86_64::Pml<4> *_pml4 = nullptr;
static Opt<x86_64::Vmm<Hal::UpperHalfMapper>> _vmm = NONE;

Hal::Vmm &vmm() {
    if (_vmm == NONE) {
        auto pml4Mem = Core::kmm()
                           .allocRange(Hal::PAGE_SIZE)
                           .unwrap("failed to allocate pml4");
        zeroFill(pml4Mem.mutBytes());
        _pml4 = pml4Mem.as<x86_64::Pml<4>>();
        _vmm = x86_64::Vmm<Hal::UpperHalfMapper>{
            Core::pmm(), _pml4};
    }

    return *_vmm;
}

void start(Core::Task &task, usize ip, usize sp, Hj::Args args) {
    Frame frame{
        .r8 = args[4],
        .rdi = args[0],
        .rsi = args[1],
        .rdx = args[2],
        .rcx = args[3],

        .rip = ip,
        .rflags = 0x202,
        .rsp = sp,
    };

    if (task.mode() == Core::TaskMode::USER) {
        frame.cs = x86_64::Gdt::UCODE * 8 | 3; // 3 = user mode
        frame.ss = x86_64::Gdt::UDATA * 8 | 3;
    } else {
        frame.cs = x86_64::Gdt::KCODE * 8;
        frame.ss = x86_64::Gdt::KDATA * 8;
    }

    task
        .stack()
        .push(frame);
}

struct Ctx : public Core::Ctx {
    usize _ksp;
    usize _usp;
    Array<Byte, Hal::PAGE_SIZE> simd __attribute__((aligned(16)));

    Ctx(usize ksp) : _ksp(ksp), _usp(0) {
        x86_64::simdInitCtx(simd.buf());
    }

    virtual void save() {
        x86_64::simdSaveCtx(simd.buf());
        x86_64::sysSetGs((usize)&_ksp);
    }

    virtual void load() {
        x86_64::simdLoadCtx(simd.buf());
    }
};

Res<Box<Core::Ctx>> createCtx(usize ksp) {
    return Ok<Box<Core::Ctx>>(makeBox<Ctx>(ksp));
}

struct ManagedVmm : public x86_64::Vmm<Hal::UpperHalfMapper> {
    ManagedVmm(x86_64::Pml<4> *pml4)
        : x86_64::Vmm<Hal::UpperHalfMapper>{Core::pmm(), pml4} {}

    ~ManagedVmm() {
        // NOTE: We expect the user to already have unmapped all the pages
        //       before destroying the vmm
        logInfo("x86_64: freeing pml4 {p}", (usize)_pml4);
        auto range = Hal::PmmRange{_mapper.unmap((usize)_pml4), Hal::PAGE_SIZE};
        _pmm.free(range).unwrap();
    }
};

Res<Strong<Hal::Vmm>> createVmm() {
    auto pml4Mem = Core::kmm()
                       .allocRange(Hal::PAGE_SIZE)
                       .unwrap("failed to allocate pml4");

    zeroFill(pml4Mem.mutBytes());
    auto *pml4 = pml4Mem.as<x86_64::Pml<4>>();

    // NOTE: Copy the kernel part of the pml4
    for (usize i = _pml4->LEN / 2; i < _pml4->LEN; i++) {
        pml4->pages[i] = _pml4->pages[i];
    }

    return Ok(makeStrong<ManagedVmm>(pml4));
}

void yield() {
    asm volatile("int $100");
}

} // namespace Hjert::Arch
