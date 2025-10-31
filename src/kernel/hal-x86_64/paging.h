#pragma once

import Karm.Core;

#include <hal/vmm.h>

namespace x86_64 {

struct [[gnu::packed]] Entry {
    static constexpr u64 PRESENT = 1 << 0;
    static constexpr u64 WRITE = 1 << 1;
    static constexpr u64 USER = 1 << 2;
    static constexpr u64 WRITE_THROUGH = 1 << 3;
    static constexpr u64 NO_CACHE = 1 << 4;
    static constexpr u64 ACCESSED = 1 << 5;
    static constexpr u64 DIRTY = 1 << 6;
    static constexpr u64 HUGE_PAGE = 1 << 7;
    static constexpr u64 GLOBAL = 1 << 8;

    static constexpr u64 PADDR_MASK = 0x000ffffffffff000;
    static constexpr u64 FLAGS_MASK = 0xfff;

    u64 _raw{};

    static u64 makeFlags(Flags<Hal::VmmFlags> flags) {
        u64 res = 0;
        if (flags.has(Hal::VmmFlags::READ)) {
            res |= WRITE;
        }
        if (flags.has(Hal::VmmFlags::WRITE)) {
            res |= WRITE;
        }
        if (flags.has(Hal::VmmFlags::EXEC)) {
            res |= 0;
        }
        if (flags.has(Hal::VmmFlags::USER)) {
            res |= USER;
        }
        if (flags.has(Hal::VmmFlags::GLOBAL)) {
            res |= GLOBAL;
        }
        if (flags.has(Hal::VmmFlags::UNCACHED)) {
            res |= NO_CACHE;
        }
        return res;
    }

    Entry() {}

    Entry(usize paddr, u64 flags) {
        _raw = (paddr & PADDR_MASK) | (flags & FLAGS_MASK);
    }

    template <typename T>
    T* as() {
        return (T*)paddr();
    }

    usize paddr() const { return _raw & PADDR_MASK; }

    u64 flags() const { return _raw & FLAGS_MASK; }

    void paddr(u64 paddr) { _raw = (paddr & PADDR_MASK) | flags(); }

    void flags(u64 flags) { _raw = (flags & FLAGS_MASK) | paddr(); }

    bool present() const { return _raw & PRESENT; }
};

static_assert(sizeof(Entry) == 8);

template <usize L>
struct [[gnu::packed]] Pml {
    constexpr static usize LEVEL = L;
    constexpr static usize LEN = 512;

    using Lower = Pml<L - 1>;

    Entry pages[LEN];

    Entry& operator[](usize i) { return pages[i]; }

    Entry const& operator[](usize i) const { return pages[i]; }

    usize virt2index(usize virt) const {
        return (virt >> (12 + (LEVEL - 1) * 9)) & 0x1ff;
    }

    usize index2virt(usize index) const {
        return index << (12 + (LEVEL - 1) * 9);
    }

    Opt<usize> virt2phys(usize virt) const {
        Entry page = pages[virt2index(virt)];

        if (not page.present()) {
            return NONE;
        }

        if (LEVEL == 1) {
            return page.paddr() + (virt & 0x1ff);
        }

        auto* pml = (Lower*)page.paddr();
        return pml->virt2phys(virt);
    }

    Entry pageAt(usize vaddr) {
        return pages[virt2index(vaddr)];
    }

    void putPage(usize vaddr, Entry page) {
        pages[virt2index(vaddr)] = page;
    }

    bool empty() const {
        for (auto const& page : pages) {
            if (page.present()) {
                return false;
            }
        }

        return true;
    }
};

static_assert(sizeof(Pml<1>) == 0x1000);

} // namespace x86_64
