#include <abi-sysv/abi.h>
#include <hjert-api/api.h>
#include <karm-main/base.h>

static int a = []() {
    debug("Hello from static");
    return 0;
}();

extern "C" void __entryPoint() {
    Abi::SysV::init();

    auto res = entryPoint({});

    auto self = Hj::Task::self();

    if (res) {
        (void)self.ret((Hj::Arg)res.none().code());
    }

    Abi::SysV::fini();
    (void)self.ret();

    while (true)
        asm volatile("pause");
}
