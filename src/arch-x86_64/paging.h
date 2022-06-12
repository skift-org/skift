#pragma once

#include <karm-base/opt.h>
#include <karm-base/std.h>

namespace x86_64 {

static constexpr size_t PAGE_SIZE = 0x1000;

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

    static constexpr uint64_t ADDR_MASK = 0x000ffffffffff000;
    static constexpr uint64_t FLAGS_MASK = 0xfff;

    uint64_t raw{};

    Entry() {}

    Entry(size_t addr, uint64_t flags) {
        raw = (addr & ADDR_MASK) | (flags & FLAGS_MASK);
    }

    template <typename T>
    T *as() {
        return (T *)addr();
    }

    size_t addr() const { return raw & ADDR_MASK; }

    void addr(uint64_t addr) { raw = (addr & ADDR_MASK) | raw; }

    uint64_t flags() const { return raw & FLAGS_MASK; }

    void flags(uint64_t flags) { raw = (flags & FLAGS_MASK) | raw; }

    bool present() const { return raw & PRESENT; }
};

static_assert(sizeof(Entry) == 8);

template <size_t L>
struct [[gnu::packed]] Pml {
    constexpr static size_t LEVEL = L;

    Entry pages[512];

    Entry &operator[](size_t i) { return pages[i]; }

    const Entry &operator[](size_t i) const { return pages[i]; }

    size_t virt2index(size_t virt) const {
        return (virt >> (LEVEL * 9)) & 0x1ff;
    }

    Opt<size_t> virt2phys(size_t virt) const {
        Entry page = pages[virt2index(virt)];

        if (!page.present()) {
            return NONE;
        }

        if (LEVEL == 1) {
            return page.addr() + (virt & 0x1ff);
        }

        auto *pml = (Pml<L - 1> *)page.addr();
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
