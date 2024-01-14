#include <hal-riscv/asm.h>
#include <sbi/sbi.h>

extern "C" Sym __kernel_start, __kernel_end;
extern "C" Sym __bss_start, __bss_end;

extern "C" void entryPoint(usize hart, usize dtb) {
    (void)hart;
    (void)dtb;
    Sbi::leacyPuts("Hello, world!\n");
    Riscv::unimp();
}

extern "C" void _intDispatch(usize) {
}
