#include <hjert/arch.h>

#include <arch-x86_64/com.h>

namespace Hjert::Arch {

x86_64::Com COM1{x86_64::Com::COM1};

Result<size_t> writeLog(void const *buf, unsigned long size) {
    COM1.write(buf, size);
    return size;
}

} // namespace Hjert::Arch
