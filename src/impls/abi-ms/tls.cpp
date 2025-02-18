// from https://github.com/XboxDev/nxdk/blob/d47653bea0d018e90b244ab7027b36f476db1735/lib/xboxrt/vcruntime/threadsafe_statics.c

extern "C" int _tls_index;
extern "C" int _Init_thread_epoch;
extern "C" int _Init_global_epoch;

int _tls_index = 0;
int _Init_thread_epoch = 0;
int _Init_global_epoch = 0;

/*
    This function tries to acquire a lock on the initialization for the static
    variable by changing the value saved in *ptss to -1. If *ptss is 0, the
    variable was not initialized yet and the function tries to set it to -1.
    If that succeeds, the function will return. If the value is already -1,
    another thread is in the process of doing the initialization and we
    wait for it. If it is any other value the initialization is complete.
    After returning the compiler generated code will check the value:
    if it is -1 it will continue with the initialization, otherwise the
    initialization must be complete and will be skipped.
*/
extern "C" void _Init_thread_header(int* ptss) {
    while (1) {
        /* Try to acquire the first initialization lock */
        // int oldTss = _InterlockedCompareExchange((long *)ptss, -1, 0);
        int expected = 0;
        int oldTss = __atomic_compare_exchange_n(ptss, &expected, -1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

        if (oldTss == -1) {
            /* Busy, wait for the other thread to do the initialization */
            continue;
        }

        if (oldTss == 0) {
            /* We acquired the lock and the caller will do the initialization */
            return;
        }

        /* The initialization is complete and the caller will skip it.
           Update the epoch so this call can be skipped in the future, it
           only needs to run once per thread. */
        _Init_thread_epoch = _Init_global_epoch;
        return;
    }
}

extern "C" void _Init_thread_footer(int* ptss) {
    /* Initialization is complete */
    _Init_thread_epoch = *ptss = __atomic_fetch_add(&_Init_global_epoch, 1, __ATOMIC_SEQ_CST);
}

extern "C" void _Init_thread_abort(int* ptss) {
    __atomic_store_n(ptss, 0, __ATOMIC_SEQ_CST);
}
