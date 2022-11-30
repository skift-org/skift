#include <hjert-core/arch.h>
#include <karm-fmt/fmt.h>

namespace Embed {

void debug(char const *buf) {
    (void)Fmt::format(Hjert::Arch::loggerOut(), "DEBUG: {}\n", buf);
}

[[noreturn]] void panic(char const *buf) {
    (void)Fmt::format(Hjert::Arch::loggerOut(), "PANIC: {}\n", buf);
    Hjert::Arch::stopAll();
}

void relaxe() { Hjert::Arch::relaxe(); }

void enterCritical() { Hjert::Arch::enterCritical(); }

void leaveCritical() { Hjert::Arch::leaveCritical(); }

} // namespace Embed
