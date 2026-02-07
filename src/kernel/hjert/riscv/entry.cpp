import Karm.Core;
import Karm.Logger;
import Vaerk.Sbi;
import Vaerk.Riscv;
import Vaerk.Dtb;
import Hjert.Core;

using namespace Karm;
using namespace Vaerk;

extern "C" char __bss_start[], __bss_end[];

namespace Hjert {

void stop() {
    for (;;)
        Riscv::wfi();
}

void entry(usize hart, void* dtbPtr) {
    yap("hjert");
    yap("hart: {}", hart);

    auto dtb = Dtb::Blob::openFromAddr(dtbPtr).unwrap();
    Io::Emit e = Arch::globalOut();
    dtb.dump(e);
}

} // namespace Hjert

void __panicHandler(PanicKind kind, char const* buf) {
    if (kind == PanicKind::PANIC) {
        Sbi::consolePuts("panic: "s);
        Sbi::consolePuts(buf);
        Sbi::consolePuts("\n");
        Hjert::stop();
        __builtin_unreachable();
    } else {
        Sbi::consolePuts("debug: "s);
        Sbi::consolePuts(buf);
        Sbi::consolePuts("\n");
    }
}

extern "C" void _hjertEntry(usize hart, usize dtbPtr) {
    std::memset(__bss_start, 0, reinterpret_cast<usize>(__bss_end) - reinterpret_cast<usize>(__bss_start));
    registerPanicHandler(__panicHandler);
    Hjert::entry(hart, (void*)dtbPtr);
    unreachable();
}