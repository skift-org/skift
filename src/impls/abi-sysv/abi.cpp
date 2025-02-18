// https://opensource.apple.com/source/libcppabi/libcppabi-14/src/cxa_guard.cxx

#include <karm-base/lock.h>

extern "C" int __cxa_atexit(void (*)(void*), void*, void*) {
    return 0;
}

extern "C" void __cxa_pure_virtual() {
    Karm::panic("pure virtual function called");
}

struct Guard {
    Atomic<u8> _guardByte;
    u8 _initByte;
    Lock _lockByte;

    bool acquire() {
        _lockByte.acquire();

        if (std::exchange(_initByte, 1) == 0)
            return 1;

        _lockByte.release();
        return 0;
    }

    void release() {
        _guardByte.store(1);
        _lockByte.release();
    }

    void abort() {
        panic("guard abort");
    }
};

static_assert(sizeof(Guard) == 3);

/**
 * This function is called before the static variable is initialized.
 * It returns 1 if the static variable should be initialized, 0 if it
 * should not be initialized.
 */
extern "C" int __cxa_guard_acquire(u64* guard) {
    return reinterpret_cast<Guard*>(guard)->acquire();
}

/**
 * This function is called after the static variable is initialized.
 */
extern "C" void __cxa_guard_release(u64* guard) {
    reinterpret_cast<Guard*>(guard)->release();
}

/**
 * This function is called if the initialization of the static variable
 * throws an exception.
 */
extern "C" void __cxa_guard_abort(u64* guard) {
    reinterpret_cast<Guard*>(guard)->abort();
}
