#include <hjert-core/arch.h>
#include <hjert-core/cpu.h>
#include <karm-fmt/fmt.h>
#include <karm-logger/logger.h>

namespace Embed {

void debug(char const *buf) {
    (void)Fmt::format(Hjert::Arch::loggerOut(), "DEBUG: {}\n", buf);
}

void backtrace(usize rbp) {
    usize *frame = reinterpret_cast<usize *>(rbp);

    while (frame) {
        usize ip = frame[1];
        usize sp = frame[0];

        logPrint("{p}", ip);

        frame = reinterpret_cast<usize *>(sp);
    }
}

usize getRbp() {
    usize rbp;
    asm volatile("mov %%rbp, %0"
                 : "=r"(rbp));
    return rbp;
}

[[noreturn]] void panic(char const *buf) {
    (void)Fmt::format(Hjert::Arch::loggerOut(), "PANIC: {}\n", buf);
    backtrace(getRbp());
    Hjert::Arch::stopAll();
}

void relaxe() { Hjert::Arch::cpu().relaxe(); }

void enterCritical() { Hjert::Arch::cpu().retainInterrupts(); }

void leaveCritical() { Hjert::Arch::cpu().releaseInterrupts(); }

} // namespace Embed
