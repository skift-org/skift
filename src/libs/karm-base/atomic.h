#pragma once

#include "std.h"

namespace Karm {

enum MemOrder {
    RELAXED = __ATOMIC_RELAXED,
    CONSUME = __ATOMIC_CONSUME,
    ACQUIRE = __ATOMIC_ACQUIRE,
    RELEASE = __ATOMIC_RELEASE,
    ACQ_REL = __ATOMIC_ACQ_REL,
    SEQ_CST = __ATOMIC_SEQ_CST
};

inline void memoryBarier(MemOrder order = SEQ_CST) {
    __atomic_thread_fence(order);
}

template <typename T>
struct Atomic {
    T _val{};

    constexpr Atomic() = default;

    Atomic(T const &val)
        : _val(val) {
    }

    Atomic(T &&val)
        : _val(std::move(val)) {
    }

    T xchg(T desired, MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_exchange_n(&_val, desired, order);
    }

    bool cmpxchg(T expected, T desired, MemOrder order = MemOrder::SEQ_CST) {
        if (order == ACQ_REL or order == RELAXED)
            return __atomic_compare_exchange_n(&_val, &expected, desired, false, RELAXED, ACQUIRE);

        return __atomic_compare_exchange_n(&_val, &expected, desired, false, order, order);
    }

    T fetchAdd(T desired, MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_fetch_add(&_val, desired, order);
    }

    T fetchSub(T desired, MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_fetch_sub(&_val, desired, order);
    }

    T fetchInc(MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_fetch_add(&_val, 1, order);
    }

    void inc(MemOrder order = MemOrder::SEQ_CST) {
        __atomic_add_fetch(&_val, 1, order);
    }

    void dec(MemOrder order = MemOrder::SEQ_CST) {
        __atomic_sub_fetch(&_val, 1, order);
    }

    T fetchDec(MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_fetch_sub(&_val, 1, order);
    }

    T load(MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_load_n(&_val, order);
    }

    void store(T desired, MemOrder order = MemOrder::SEQ_CST) {
        __atomic_store_n(&_val, desired, order);
    }

    bool lockFree() {
        return __atomic_is_lock_free(&_val);
    }
};

}; // namespace Karm
