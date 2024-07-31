#pragma once

#include <hal/mem.h>
#include <hal/pmm.h>
#include <hal/vmm.h>
#include <karm-logger/logger.h>

#include "asm.h"
#include "paging.h"

namespace x86_64 {

template <typename Mapper = Hal::IdentityMapper>
struct Vmm : public Hal::Vmm {
    Hal::Pmm &_pmm;
    Pml<4> *_pml4 = nullptr;
    Mapper _mapper;

    Vmm(Hal::Pmm &pmm, Pml<4> *pml4, Mapper mapper = {})
        : _pmm(pmm),
          _pml4(pml4),
          _mapper(mapper) {}

    template <usize L>
    Res<Pml<L - 1> *> pml(Pml<L> &upper, usize vaddr) {
        auto page = upper.pageAt(vaddr);

        if (not page.present())
            return Error::invalidInput("page not present");

        return Ok(_mapper.map(page.template as<Pml<L - 1>>()));
    }

    template <usize L>
    Res<Pml<L - 1> *> pmlOrAlloc(Pml<L> &upper, usize vaddr) {
        auto page = upper.pageAt(vaddr);

        if (page.present()) {
            return Ok(_mapper.map(page.template as<Pml<L - 1>>()));
        }

        usize lower = try$(_pmm.allocRange(Hal::PAGE_SIZE, Hal::PmmFlags::NONE)).start;
        memset(_mapper.map((void *)lower), 0, Hal::PAGE_SIZE);
        upper.putPage(vaddr, {lower, Entry::WRITE | Entry::PRESENT | Entry::USER});
        return Ok(_mapper.map((Pml<L - 1> *)lower));
    }

    Res<> allocPage(usize vaddr, usize paddr, Hal::VmmFlags flags) {
        auto pml3 = try$(pmlOrAlloc(*_pml4, vaddr));
        auto pml2 = try$(pmlOrAlloc(*pml3, vaddr));
        auto pml1 = try$(pmlOrAlloc(*pml2, vaddr));

        pml1->putPage(vaddr, {paddr, Entry::makeFlags(flags) | Entry::PRESENT});

        return Ok();
    }

    Res<> freePage(usize vaddr) {
        auto pml3 = try$(pml(*_pml4, vaddr));
        auto pml2 = try$(pml(*pml3, vaddr));
        auto pml1 = try$(pml(*pml2, vaddr));
        pml1->putPage(vaddr, {});

        if (pml1->empty()) {
            pml2->putPage(vaddr, {});
            try$(_pmm.free({_mapper.unmap((usize)pml1), Hal::PAGE_SIZE}));
        }

        if (pml2->empty()) {
            pml3->putPage(vaddr, {});
            try$(_pmm.free({_mapper.unmap((usize)pml2), Hal::PAGE_SIZE}));
        }

        if (pml3->empty()) {
            _pml4->putPage(vaddr, {});
            try$(_pmm.free({_mapper.unmap((usize)pml3), Hal::PAGE_SIZE}));
        }

        return Ok();
    }

    Res<Hal::VmmRange> mapRange(Hal::VmmRange vaddr, Hal::PmmRange paddr, Hal::VmmFlags flags) override {
        if (paddr.size != vaddr.size) {
            return Error::invalidInput();
        }

        for (usize page = 0; page < vaddr.size; page += Hal::PAGE_SIZE) {
            try$(allocPage(vaddr.start + page, paddr.start + page, flags));
        }

        return Ok(vaddr);
    }

    Res<> free(Hal::VmmRange vaddr) override {
        for (usize page = 0; page < vaddr.size; page += Hal::PAGE_SIZE) {
            try$(freePage(vaddr.start + page));
        }

        return Ok();
    }

    Res<> update(Hal::VmmRange, Hal::VmmFlags) override {
        notImplemented();
    }

    Res<> flush(Hal::VmmRange vaddr) override {
        for (usize i = 0; i < vaddr.size; i += Hal::PAGE_SIZE) {
            x86_64::invlpg(vaddr.start + i);
        }

        return Ok();
    }

    void activate() override {
        x86_64::wrcr3(root());
    }

    struct Context {
        usize vstart = -1;
        usize vend = 0;

        usize pstart = 0;
        usize pend = 0;

        void next(usize vaddr, usize paddr) {
            if (vstart == (usize)-1) {
                vstart = vaddr;
                vend = vaddr;

                pstart = paddr;
                pend = paddr;
                return;
            }

            if ((vend + Hal::PAGE_SIZE != vaddr) or
                (pend + Hal::PAGE_SIZE != paddr)) {

                logInfo("x86_64: vmm: {x}-{x} {x}-{x}", vstart, vend + Hal::PAGE_SIZE, pstart, pend + Hal::PAGE_SIZE);
                vstart = vaddr;
                vend = vaddr;

                pstart = paddr;
                pend = paddr;
            } else {
                vend += Hal::PAGE_SIZE;
                pend += Hal::PAGE_SIZE;
            }
        }
    };

    template <usize L>
    void _dumpPml(Context &ctx, Pml<L> &pml, usize vaddr) {
        for (usize i = 0; i < 512; i++) {
            auto page = pml[i];
            usize curr = pml.index2virt(i) | vaddr;
            if (page.present()) {
                if constexpr (L == 1) {
                    ctx.next(curr, page.paddr());
                } else {
                    auto &lower = *_mapper.map(page.template as<Pml<L - 1>>());
                    _dumpPml(ctx, lower, curr);
                }
            }
        }
    }

    void dump() override {
        logInfo("x86_64: vmm: dump pml4 {x}", (usize)_pml4);
        Context ctx{};
        _dumpPml(ctx, *_pml4, 0);
        ctx.next(0, 0);
    }

    usize root() override {
        return _mapper.unmap((usize)_pml4);
    }
};

} // namespace x86_64
