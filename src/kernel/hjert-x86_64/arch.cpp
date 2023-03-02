#include <hjert-core/arch.h>
#include <hjert-core/cpu.h>
#include <hjert-core/mem.h>
#include <hjert-core/sched.h>
#include <hjert-core/syscalls.h>
#include <karm-logger/logger.h>

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

Io::TextWriter<> &loggerOut() {
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

extern "C" usize _intDispatch(usize sp) {
    auto *frame = reinterpret_cast<Frame *>(sp);

    cpu().beginInterrupt();

    if (frame->intNo < 32) {
        logFatal("x86_64: cpu exception: {} (err={}, ip={x}, sp={x}, cr2={x}, cr3={x})", _faultMsg[frame->intNo], frame->errNo, frame->rip, frame->rsp, x86_64::rdcr2(), x86_64::rdcr3());
    } else {
        isize irq = frame->intNo - 32;

        if (irq == 0) {
            Core::Task::self().saveCtx(sp);
            Core::Sched::self().schedule();
            sp = Core::Task::self().loadCtx();
        } else {
            logInfo("x86_64: irq: {}", irq);
        }
    }

    _pic.ack(frame->intNo);

    cpu().endInterrupt();

    return sp;
}

extern "C" usize _sysDispatch(usize sp) {
    auto *frame = reinterpret_cast<Frame *>(sp);
    return (usize)Core::dispatchSyscall(
        (Hj::Syscall)frame->rax,
        {frame->rdi, frame->rsi, frame->rdx, frame->r10, frame->r8, frame->r9});
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

    if (task.type() == Core::TaskType::USER) {
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

template <typename L, typename M>
Res<> destroyPml(Hal::Pmm &pmm, L *pml, M mapper = {}) {
    logInfo("x86_64: destroying pml{} at {x}", L::LEVEL, (usize)pml);

    auto range = Hal::PmmRange{mapper.unmap((usize)pml), Hal::PAGE_SIZE};

    // NOTE: we only need to free the first half of the pml4 since hupper is for the kernel
    for (usize i = 0; i < L::LEN / (L::LEVEL == 4 ? 2 : 1); i++) {
        if (pml->pages[i].present()) {
            if constexpr (L::LEVEL == 1) {
                auto page = Hal::PmmRange{mapper.map(pml->pages[i].paddr()), Hal::PAGE_SIZE};
                try$(pmm.free(page));
            } else {
                try$(destroyPml(pmm, (typename L::Lower *)mapper.map(pml->pages[i].paddr()), mapper));
            }
        }
    }

    try$(pmm.free(range));

    return Ok();
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

struct Space :
    public Core::Space {

    x86_64::Vmm<Hal::UpperHalfMapper> _vmm;
    x86_64::Pml<4> *_pml4;

    Space(x86_64::Pml<4> *pml4)
        : _vmm{Core::pmm(), pml4}, _pml4(pml4) {}

    Space(Space &&other)
        : _vmm(other._vmm),
          _pml4(std::exchange(other._pml4, nullptr)) {
    }

    ~Space() {
        destroyPml(Core::pmm(), _pml4, Hal::UpperHalfMapper{})
            .unwrap("failed to destroy pml4");
    }

    Hal::Vmm &vmm() override {
        return _vmm;
    }
};

Res<Strong<Core::Space>> createSpace() {
    auto pml4Mem = Core::kmm()
                       .allocRange(Hal::PAGE_SIZE)
                       .unwrap("failed to allocate pml4");

    zeroFill(pml4Mem.mutBytes());
    auto *pml4 = pml4Mem.as<x86_64::Pml<4>>();

    // Copy the kernel part of the pml4
    for (usize i = _pml4->LEN / 2; i < _pml4->LEN; i++) {
        pml4->pages[i] = _pml4->pages[i];
    }

    return Ok(makeStrong<Space>(pml4));
}

} // namespace Hjert::Arch
