#include <abi-sysv/abi.h>
#include <hjert-api/api.h>
#include <karm-main/base.h>

extern "C" void __entryPoint() {
    Abi::SysV::init();

    auto res = entryPoint({});

    auto self = Hj::Task::self();

    if (not res) {
        (void)self.ret(-(isize)res.none().code());
    }

    Abi::SysV::fini();
    (void)self.ret();

    while (true)
        asm volatile("pause");
}
