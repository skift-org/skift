#include <hjert/arch.h>

namespace Embed {

void debugHandler(char const *buf) {
    Hjert::Arch::writeLog("DEBUG: ").unwrap();
    Hjert::Arch::writeLog(buf).unwrap();
    Hjert::Arch::writeLog("\n").unwrap();
}

[[noreturn]] void panicHandler(char const *buf) {
    Hjert::Arch::writeLog("PANIC: ").unwrap();
    Hjert::Arch::writeLog(buf).unwrap();
    Hjert::Arch::writeLog("\n").unwrap();

    Hjert::Arch::stopCpu();
}

} // namespace Embed
