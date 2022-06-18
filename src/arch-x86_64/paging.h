#pragma once

#include <karm-base/opt.h>
#include <karm-base/std.h>

namespace x86_64 {

struct [[gnu::packed]] Entry {
    static constexpr uint64_t PRESENT = 1 << 0;
    static constexpr uint64_t WRITE = 1 << 1;
    static constexpr uint64_t USER = 1 << 2;
    static constexpr uint64_t WRITE_THROUGH = 1 << 3;
    static constexpr uint64_t NO_CACHE = 1 << 4;
    static constexpr uint64_t ACCESSED = 1 << 5;
    static constexpr uint64_t DIRTY = 1 << 6;
    static constexpr uint64_t HUGE_PAGE = 1 << 7;
    static constexpr uint64_t GLOBAL = 1 << 8;

    static constexpr uint64_t PADDR_MASK = 0x000ffffffffff000;
    static constexpr uint64_t FLAGS_MASK = 0xfff;

    uint64_t _raw{};

    Entry() {}

    Entry(size_t paddr, uint64_t flags) {
        _raw = (paddr & PADDR_MASK) | (flags & FLAGS_MASK);
    }

    template <typename T>
    T *as() {
        return (T *)paddr();
    }

    size_t paddr() const { return _raw & PADDR_MASK; }

    uint64_t flags() const { return _raw & FLAGS_MASK; }

    void paddr(uint64_t paddr) { _raw = (paddr & PADDR_MASK) | flags(); }

    void flags(uint64_t flags) { _raw = (flags & FLAGS_MASK) | paddr(); }

    bool present() const { return _raw & PRESENT; }
};

static_assert(sizeof(Entry) == 8);

template <size_t L>
struct [[gnu::packed]] Pml {
    constexpr static size_t LEVEL = L;

    Entry pages[512];

    Entry &operator[](size_t i) { return pages[i]; }

    const Entry &operator[](size_t i) const { return pages[i]; }

    size_t virt2index(size_t virt) const {
        return (virt >> (12 + (LEVEL - 1) * 9)) & 0x1ff;
    }

    size_t index2virt(size_t index) const {
        return (index) << (12 + (LEVEL - 1) * 9);
    }

    Opt<size_t> virt2phys(size_t virt) const {
        Entry page = pages[virt2index(virt)];

        if (!page.present()) {
            return NONE;
        }

        if (LEVEL == 1) {
            return page.paddr() + (virt & 0x1ff);
        }

        auto *pml = (Pml<L - 1> *)page.paddr();
        return pml->virt2phys(virt);
    }

    Entry pageAt(size_t vaddr) {
        return pages[virt2index(vaddr)];
    }

    void putPage(size_t vaddr, Entry page) {
        pages[virt2index(vaddr)] = page;
    }
};

static_assert(sizeof(Pml<1>) == 0x1000);

} // namespace x86_64
