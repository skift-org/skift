#include <karm-base/panic.h>

extern "C" int __cxa_atexit(void (*)(void *), void *, void *) {
    return 0;
}

extern "C" void __cxa_pure_virtual() {
    Karm::panic("pure virtual function called");
}

// __cxa_guard_acquire

extern "C" int __cxa_guard_acquire(int *) {
    // FIXME: implement me
    return 0;
}

extern "C" void __cxa_guard_release(int *) {
}
