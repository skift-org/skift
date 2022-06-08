#pragma once

#include <hal/pmm.h>
#include <hal/vmm.h>

#include "paging.h"

namespace x86_64 {

struct Vmm : public Hal::Vmm {
    Hal::Pmm &_pmm;
    Pml<4> *_pml4;

    Vmm(Hal::Pmm &pmm) : _pmm(pmm) {}

    Error map(PmmRange paddr, VmmRange vaddr, VmmFlags flags) override {
    }

    Error unmap(VmmRange vaddr) override {
    }

    Error update(VmmRange vaddr, VmmFlags flags) override {
    }

    Error flush(VmmRange vaddr) override {
    }

    virtual void activate() override {
    }
};

} // namespace x86_64
