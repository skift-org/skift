#pragma once

#include <hal/mem.h>
#include <hal/pmm.h>
#include <hal/vmm.h>
#include <karm-sys/chan.h>

#include "asm.h"
#include "paging.h"

namespace x86_64 {

struct Vmm : public Hal::Vmm {
    Hal::Pmm &_pmm;
    Pml<4> *_pml4 = nullptr;

    Vmm(Hal::Pmm &pmm, Pml<4> *pml4)
        : _pmm(pmm),
          _pml4(pml4) {}

    template <size_t L>
    Result<Pml<L> *> pml(Pml<L + 1> &upper, size_t vaddr) {
        auto page = upper.pageAt(vaddr);

        if (page.present()) {
            return page.template as<Pml<L>>();
        }

        return Error::ADDR_NOT_AVAILABLE;
    }

    template <size_t L>
    Result<Pml<L> *> pmlOrAlloc(Pml<L + 1> &upper, size_t vaddr) {
        auto page = upper.pageAt(vaddr);

        if (page.present()) {
            return page.template as<Pml<L>>();
        }

        size_t lower = try$(_pmm.alloc(Hal::PAGE_SIZE, Hal::PmmFlags::NIL)).start();
        memset((void *)lower, 0, Hal::PAGE_SIZE);
        upper.putPage(vaddr, {lower, Entry::WRITE | Entry::PRESENT | Entry::USER});
        return (Pml<L> *)lower;
    }

    Error mapPage(size_t vaddr, size_t paddr, Hal::VmmFlags) {
        auto pml3 = try$(pmlOrAlloc<3>(*_pml4, vaddr));
        auto pml2 = try$(pmlOrAlloc<2>(*pml3, vaddr));
        auto pml1 = try$(pmlOrAlloc<1>(*pml2, vaddr));
        pml1->putPage(vaddr, {paddr, Entry::WRITE | Entry::PRESENT | Entry::USER});
        return OK;
    }

    Error unmapPage(size_t vaddr) {
        auto pml3 = try$(pml<3>(*_pml4, vaddr));
        auto pml2 = try$(pml<2>(*pml3, vaddr));
        auto pml1 = try$(pml<1>(*pml2, vaddr));
        pml1->putPage(vaddr, {});
        return OK;
    }

    Error map(Hal::VmmRange vaddr, Hal::PmmRange paddr, Hal::VmmFlags flags) override {
        if (paddr.size() != vaddr.size()) {
            return Error::INVALID_INPUT;
        }

        for (size_t page = 0; page < vaddr.size(); page += Hal::PAGE_SIZE) {
            try$(mapPage(vaddr.start() + page, paddr.start() + page, flags));
        }
        return OK;
    }

    Error unmap(Hal::VmmRange vaddr) override {
        for (size_t page = 0; page < vaddr.size(); page += Hal::PAGE_SIZE) {
            try$(unmapPage(vaddr.start() + page));
        }
        return OK;
    }

    Error update(Hal::VmmRange, Hal::VmmFlags) override {
        notImplemented();
    }

    Error flush(Hal::VmmRange vaddr) override {
        for (size_t i = 0; i < vaddr.size(); i += Hal::PAGE_SIZE) {
            x86_64::invlpg(vaddr.start() + i);
        }

        return OK;
    }

    void activate() override {
        x86_64::wrcr3((uint64_t)_pml4);
    }

    template <size_t LEVEL>
    void _dumpPml(Pml<LEVEL> &pml, size_t vaddr) {
        for (size_t i = 0; i < 512; i++) {
            auto page = pml[i];
            size_t curr = pml.index2virt(i) | vaddr;
            if constexpr (LEVEL == 1) {
                if (page.present()) {
                    Sys::println("{x} {x}", curr, page._raw);
                }
            } else if (page.present()) {
                auto &lower = *page.template as<Pml<LEVEL - 1>>();
                _dumpPml(lower, curr);
            }
        }
    }

    void dump() override {
        _dumpPml(*_pml4, 0);
    }

    size_t root() override {
        return (size_t)_pml4;
    }
};

} // namespace x86_64
