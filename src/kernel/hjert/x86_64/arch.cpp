module;

#include <hal-x86_64/com.h>
#include <hal-x86_64/gdt.h>
#include <hal-x86_64/idt.h>
#include <hal-x86_64/pic.h>
#include <hal-x86_64/pit.h>
#include <hal-x86_64/simd.h>
#include <hal-x86_64/sys.h>
#include <hal-x86_64/vmm.h>
#include <vaerk-handover/spec.h>

module Hjert.Core;

import Karm.Core;
import Karm.Tty;

namespace Hjert::Arch {

struct [[gnu::packed]] Frame {
    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 r11;
    u64 r10;
    u64 r9;
    u64 r8;
    u64 rbp;
    u64 rdi;
    u64 rsi;
    u64 rdx;
    u64 rcx;
    u64 rbx;
    u64 rax;

    u64 intNo;
    u64 errNo;

    u64 rip;
    u64 cs;
    u64 rflags;
    u64 rsp;
    u64 ss;
};

extern "C" Array<usize, 256> _intVec;

extern "C" void _intDispatch(usize rsp);

extern "C" void _sysHandler();

extern "C" usize _sysDispatch(usize rsp);

static x86_64::Com _com1 = x86_64::Com::com1();

static x86_64::DualPic _pic = x86_64::DualPic::dualPic();
static x86_64::Pit _pit = x86_64::Pit::pit();

static Array<u8, Hal::PAGE_SIZE * 16> _kstack{};
static x86_64::Tss _tss{};

static x86_64::Gdt _gdt{_tss};
static x86_64::GdtDesc _gdtDesc{_gdt};

static x86_64::Idt _idt{};
static x86_64::IdtDesc _idtDesc{_idt};

Res<> init(Handover::Payload&) {
    try$(_com1.init());

    _gdtDesc.load();
    _tss = {};
    _tss.rsp[0] = (u64)_kstack.bytes().end();
    x86_64::_tssUpdate();

    for (usize i = 0; i < x86_64::Idt::LEN; i++) {
        _idt.entries[i] = x86_64::IdtEntry{_intVec[i], 0, x86_64::IdtEntry::GATE};
    }

    _idtDesc.load();

    try$(_pic.init());
    try$(_pit.init(1000));

    x86_64::simdInit();
    x86_64::sysInit(_sysHandler);

    return Ok();
}

Io::TextWriter& globalOut() {
    return _com1;
}

void stop() {
    while (true) {
        x86_64::cli();
        x86_64::hlt();
    }
}

// MARK: Cpu -------------------------------------------------------------------

struct Cpu : Core::Cpu {
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

Core::Cpu& globalCpu() {
    return _cpu;
}

// MARK: Interrupts ------------------------------------------------------------

static char const* _faultMsg[32] = {
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
    usize* frame = reinterpret_cast<usize*>(rbp);

    while (frame) {
        usize ip = frame[1];
        usize sp = frame[0];

        logPrint("    ip={p}, sp={p}", ip, sp);

        frame = reinterpret_cast<usize*>(sp);
    }
}

void switchTask(Duration span, Frame& frame) {
    Core::globalSched().currentTask().save(frame);
    Core::clockTick(span);
    Core::globalSched().schedule();
    Core::globalSched().currentTask().load(frame);
}

void uPanic(Frame& frame) {
    auto& task = Core::globalSched().currentTask();
    logError("{} caused a '{}'", task, _faultMsg[frame.intNo]);
    logError("int={} err={} rip={p} rsp={p} rbp={p} cr2={p} cr3={p}", frame.intNo, frame.errNo, frame.rip, frame.rsp, frame.rbp, x86_64::rdcr2(), x86_64::rdcr3());
    task.space().dump();
    task.crash();
    switchTask(0_ms, frame);
}

void kPanic(Frame& frame) {
    logPrint("{}--- {} {}----------------------------------------------------", Tty::style(Tty::YELLOW_LIGHT), "!!!" | Tty::Style(Tty::RED).bold(), Tty::style(Tty::YELLOW_LIGHT));
    logPrint("");
    logPrint("    {}", "Kernel Panic" | Tty::style(Tty::RED).bold());
    logPrint("    kernel cause a '{}'", _faultMsg[frame.intNo]);
    logPrint("    {}", witty(frame.rsp + frame.rip) | Tty::GRAY_DARK);
    logPrint("");
    logPrint("    {}", "Registers" | Tty::WHITE);
    logPrint("    int={}", frame.intNo);
    logPrint("    err={}", frame.errNo);
    logPrint("    rip={p}", frame.rip);
    logPrint("    rbp={p}", frame.rbp);
    logPrint("    rsp={p}", frame.rsp);
    logPrint("    cr2={p}", x86_64::rdcr2());
    logPrint("    cr3={p}", x86_64::rdcr3());
    logPrint("");
    logPrint("    {}", "Backtrace" | Tty::WHITE);
    backtrace(frame.rbp);
    logPrint("");
    logPrint("    {}", "System halted" | Tty::WHITE);
    logPrint("");
    logPrint("{}", "-----------------------------------------------------------" | Tty::YELLOW_LIGHT);

    panic("cpu exception");
}

extern "C" void _intDispatch(usize sp) {
    auto& frame = *reinterpret_cast<Frame*>(sp);

    globalCpu().beginInterrupt();

    if (frame.intNo < 32) {
        if (frame.cs == (x86_64::Gdt::UCODE * 8 | 3))
            uPanic(frame);
        else
            kPanic(frame);
    } else if (frame.intNo == 100) {
        switchTask(0_ms, frame);
    } else {
        isize irq = frame.intNo - 32;

        if (irq == 0)
            switchTask(1_ms, frame);
        else
            Core::Irq::dispatch(irq);

        _pic.ack(frame.intNo)
            .unwrap("pic ack failed");
    }

    globalCpu().endInterrupt();
}

void yield() {
    asm volatile("int $100");
}

// MARK: Syscalls --------------------------------------------------------------

extern "C" usize _sysDispatch(usize sp) {
    auto* frame = reinterpret_cast<Frame*>(sp);

    auto result = Core::doSyscall(
        (Hj::Syscall)frame->rax,
        {
            frame->rdi,
            frame->rsi,
            frame->rdx,
            frame->r10,
            frame->r8,
            frame->r9,
        }
    );

    if (not result) {
        return (usize)result.none().code();
    }

    return (usize)Error::_OK;
}

// MARK: Vmm -------------------------------------------------------------------

static x86_64::Pml<4>* _kpml4 = nullptr;
static Opt<x86_64::Vmm<Hal::UpperHalfMapper>> _vmm = NONE;

Hal::Vmm& globalVmm() {
    if (_vmm == NONE) {
        auto pml4Mem = Core::kmm()
                           .allocRange(Hal::PAGE_SIZE)
                           .unwrap("failed to allocate pml4");
        zeroFill(pml4Mem.mutBytes());
        _kpml4 = pml4Mem.as<x86_64::Pml<4>>();
        _vmm = x86_64::Vmm<Hal::UpperHalfMapper>{
            Core::pmm(), _kpml4
        };
    }

    return *_vmm;
}

struct UserVmm : x86_64::Vmm<Hal::UpperHalfMapper> {
    UserVmm(x86_64::Pml<4>* pml4)
        : x86_64::Vmm<Hal::UpperHalfMapper>{Core::pmm(), pml4} {}

    ~UserVmm() {
        // NOTE: We expect the user to already have unmapped all the pages
        //       before destroying the vmm
        logInfo("x86_64: freeing pml4 {p}", (usize)_pml4);
        auto range = Hal::PmmRange{_mapper.unmap((usize)_pml4), Hal::PAGE_SIZE};
        _pmm.free(range).unwrap();
    }
};

Res<Arc<Hal::Vmm>> createVmm() {
    auto pml4Mem = Core::kmm()
                       .allocRange(Hal::PAGE_SIZE)
                       .unwrap("failed to allocate pml4");

    zeroFill(pml4Mem.mutBytes());
    auto* pml4 = pml4Mem.as<x86_64::Pml<4>>();

    // NOTE: Copy the kernel part of the pml4
    for (usize i = _kpml4->LEN / 2; i < _kpml4->LEN; i++) {
        pml4->pages[i] = _kpml4->pages[i];
    }

    return Ok(makeArc<UserVmm>(pml4));
}

// MARK: Tasking ---------------------------------------------------------------

struct Context : Core::Context {
    usize _ksp;
    usize _usp;

    Frame _frame;
    Array<u8, 1024> _simd __attribute__((aligned(16)));

    Context(usize ksp)
        : _ksp(ksp), _usp(0) {
        x86_64::simdInitContext(_simd.buf());
    }

    void save(Arch::Frame const& frame) override {
        x86_64::simdSaveContext(_simd.buf());
        _frame = frame;
    }

    void load(Arch::Frame& frame) override {
        frame = _frame;
        x86_64::simdLoadContext(_simd.buf());
        x86_64::sysSetGs((usize)&_ksp);
    }
};

Res<Box<Core::Context>> createContext(Hj::Mode mode, usize ip, usize sp, usize ksp, Hj::Args args) {
    Frame frame{
        .r15 = 0,
        .r14 = 0,
        .r13 = 0,
        .r12 = 0,
        .r11 = 0,
        .r10 = 0,
        .r9 = args[5],
        .r8 = args[4],
        .rbp = 0,
        .rdi = args[0],
        .rsi = args[1],
        .rdx = args[2],
        .rcx = args[3],
        .rbx = 0,
        .rax = 0,
        .intNo = 0,
        .errNo = 0,

        .rip = ip,
        .cs = 0,
        .rflags = 0x202,
        .rsp = sp,
        .ss = 0,
    };

    if (mode == Hj::Mode::USER) {
        frame.cs = x86_64::Gdt::UCODE * 8 | 3; // 3 = user mode
        frame.ss = x86_64::Gdt::UDATA * 8 | 3;
    } else {
        frame.cs = x86_64::Gdt::KCODE * 8;
        frame.ss = x86_64::Gdt::KDATA * 8;
    }

    auto ctx = makeBox<Context>(ksp);
    ctx->_frame = frame;
    return Ok<Box<Core::Context>>(std::move(ctx));
}

} // namespace Hjert::Arch
