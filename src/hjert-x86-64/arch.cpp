#include <hjert/arch.h>

#include <arch-x86_64/com.h>

namespace Hjert::Arch {

Opt<x86_64::Com> _com1;

Result<size_t> writeLog(Str str) {
    if (!_com1) {
        _com1 = x86_64::Com{x86_64::Com::COM1};
        _com1->init();
    }

    return _com1->writeStr(str);
}

void stopCpu() {
    while (true) {
        x86_64::cli();
        x86_64::hlt();
    }
}

} // namespace Hjert::Arch
