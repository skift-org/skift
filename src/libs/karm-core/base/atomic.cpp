module;

#include <karm-core/macros.h>

export module Karm.Core:base.atomic;

import :base.base;
import :base.enum_;

namespace Karm {

export enum MemOrder {
    RELAXED = __ATOMIC_RELAXED,
    CONSUME = __ATOMIC_CONSUME,
    ACQUIRE = __ATOMIC_ACQUIRE,
    RELEASE = __ATOMIC_RELEASE,
    ACQ_REL = __ATOMIC_ACQ_REL,
    SEQ_CST = __ATOMIC_SEQ_CST
};

export void memoryBarier(MemOrder order = SEQ_CST) {
    __atomic_thread_fence(order);
}

export template <typename T>
struct Atomic {
    T _val{};

    always_inline constexpr Atomic() = default;

    always_inline Atomic(T const& val)
        : _val(val) {
    }

    always_inline Atomic(T&& val)
        : _val(std::move(val)) {
    }

    always_inline T xchg(T desired, MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_exchange_n(&_val, desired, order);
    }

    always_inline bool cmpxchg(T expected, T desired, MemOrder order = MemOrder::SEQ_CST) {
        if (order == MemOrder::ACQ_REL or order == MemOrder::RELAXED)
            return __atomic_compare_exchange_n(&_val, &expected, desired, false, MemOrder::RELAXED, MemOrder::ACQUIRE);

        return __atomic_compare_exchange_n(&_val, &expected, desired, false, order, order);
    }

    always_inline T fetchAdd(T desired, MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_fetch_add(&_val, desired, order);
    }

    always_inline T fetchSub(T desired, MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_fetch_sub(&_val, desired, order);
    }

    always_inline T fetchInc(MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_fetch_add(&_val, 1, order);
    }

    always_inline void inc(MemOrder order = MemOrder::SEQ_CST) {
        __atomic_add_fetch(&_val, 1, order);
    }

    always_inline void dec(MemOrder order = MemOrder::SEQ_CST) {
        __atomic_sub_fetch(&_val, 1, order);
    }

    always_inline T fetchDec(MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_fetch_sub(&_val, 1, order);
    }

    always_inline T load(MemOrder order = MemOrder::SEQ_CST) {
        return __atomic_load_n(&_val, order);
    }

    always_inline void store(T desired, MemOrder order = MemOrder::SEQ_CST) {
        __atomic_store_n(&_val, desired, order);
    }

    always_inline bool lockFree() {
        return __atomic_is_lock_free(&_val);
    }
};

} // namespace Karm
