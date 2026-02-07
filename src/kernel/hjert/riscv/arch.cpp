module;

#include <hal/vmm.h>
#include <vaerk-handover/spec.h>

module Hjert.Core;

import Vaerk.Sbi;
import Vaerk.Riscv;

using namespace Vaerk;

namespace Hjert::Arch {

struct Frame;

struct Cpu : Core::Cpu {
    void enableInterrupts() override {
        Riscv::csrrs(Riscv::Csr::MSTATUS, 0x80);
    }

    void disableInterrupts() override {
        Riscv::csrrc(Riscv::Csr::MSTATUS, 0x80);
    }

    void relaxe() override {
        Riscv::wfi();
    }
};

static Cpu _cpu{};

Core::Cpu& globalCpu() {
    return _cpu;
}

Hal::Vmm& globalVmm() {
    notImplemented();
}

struct SbiConsole : Io::TextWriter {
    Res<> writeRune(Rune rune) override {
        Sbi::consolePutchar(rune);
        return Ok();
    }
};

static SbiConsole _out{};

Io::TextWriter& globalOut() {
    return _out;
}

Res<Box<Core::Context>> createContext(Hj::Mode, usize, usize, usize, Hj::Args) {
    notImplemented();
}

Res<Arc<Hal::Vmm>> createVmm() {
    notImplemented();
}

Res<> init(Handover::Payload&) {
    notImplemented();
}

[[noreturn]] void stop() {
    for (;;)
        Riscv::wfi();
}

void yield() {
    notImplemented();
}

} // namespace Hjert::Arch