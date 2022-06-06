#pragma once

#include <karm-base/opt.h>
#include <karm-base/std.h>

namespace x86_64 {

static constexpr size_t PAGE_SIZE = 0x1000;

struct [[gnu::packed]] Page {

    static constexpr uint64_t PRESENT = 1;
    static constexpr uint64_t WRITE = 1 << 1;
    static constexpr uint64_t USER = 1 << 2;
    static constexpr uint64_t WRITE_THROUGH = 1 << 3;
    static constexpr uint64_t NO_CACHE = 1 << 4;
    static constexpr uint64_t ACCESSED = 1 << 5;
    static constexpr uint64_t DIRTY = 1 << 6;
    static constexpr uint64_t HUGE_PAGE = 1 << 7;
    static constexpr uint64_t GLOBAL = 1 << 8;

    static constexpr uint64_t ADDR_MASK = 0x000ffffffffff000;
    static constexpr uint64_t FLAGS_MASK = 0xfff;

    uint64_t raw;

    Page() {}

    Page(uintptr_t addr, uint64_t flags) {
        this->addr(addr);
        this->flags(flags);
    }

    uintptr_t addr() const { return raw & ADDR_MASK; }

    uint64_t flags() const { return raw & FLAGS_MASK; }

    void addr(uint64_t addr) { raw = (raw & ~ADDR_MASK) | addr; }

    void flags(uint64_t flags) { raw = (raw & ~FLAGS_MASK) | flags; }

    bool present() const { return raw & PRESENT; }

    void clear() { raw = 0; }
};

template <size_t L>
struct [[gnu::packed]] Pml {
    constexpr static size_t LEVEL = L;

    Page pages[512];

    Page &operator[](size_t i) { return pages[i]; }

    const Page &operator[](size_t i) const { return pages[i]; }

    size_t virt2index(uintptr_t virt) const {
        return (virt >> (LEVEL * 9)) & 0x1ff;
    }

    Opt<uintptr_t> virt2phys(uintptr_t virt) const {
        Page page = pages[virt2index(virt)];

        if (!page.present()) {
            return NONE;
        }

        if (LEVEL == 1) {
            return page.addr() + (virt & 0x1ff);
        }

        auto *pml = (Pml<L - 1> *)page.addr();
        return pml->virt2phys(virt);
    }
};



} // namespace x86_64
