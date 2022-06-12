#include <hjert/arch.h>

#include <arch-x86_64/com.h>

namespace Hjert::Arch {

Opt<x86_64::Com> _com1;

Result<size_t> writeLog(void const *buf, unsigned long size) {
    if (!_com1) {
        _com1 = x86_64::Com{x86_64::Com::COM1};
    }
    _com1->write(buf, size);
    return size;
}

void stopCpu() {
    while (true) {
        x86_64::cli();
        x86_64::hlt();
    }
}

} // namespace Hjert::Arch
