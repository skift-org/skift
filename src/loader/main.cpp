#include <karm-main/main.h>
#include <karm-sys/file.h>

ExitCode entryPoint(CliArgs const &) {
    Sys::println("Loading kernel...");
    Sys::File kerneFile = try$(Sys::File::open("/EFI/BOOT/hjert.elf"));

    for (;;)
        ;
    return Cli::SUCCESS;
}
