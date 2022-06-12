#include <hjert/arch.h>

namespace Embed {

void debugHandler(char const *buf) {
    Hjert::Arch::writeLog("DEBUG: ", 7).unwrap();
    Hjert::Arch::writeLog(buf, strlen(buf)).unwrap();
    Hjert::Arch::writeLog("\n", 1).unwrap();
}

[[noreturn]] void panicHandler(char const *buf) {
    Hjert::Arch::writeLog("PANIC: ", 7).unwrap();
    Hjert::Arch::writeLog(buf, strlen(buf)).unwrap();
    Hjert::Arch::writeLog("\n", 1).unwrap();

    while (1)
        ;
}

} // namespace Embed
