#include <karm-cli/style.h>
#include <karm-main/main.h>
#include <karm-sys/info.h>

CliResult entryPoint(CliArgs) {
    auto userinfo = try$(Sys::userinfo());
    auto sysinfo = try$(Sys::sysinfo());
    auto meminfo = try$(Sys::meminfo());
    auto cpusinfo = try$(Sys::cpusinfo());

    Sys::println("User: {} {}", userinfo.name, userinfo.home);

    Sys::println("System: {} {}", sysinfo.sysName, sysinfo.sysVersion);
    Sys::println("Kernel: {} {}", sysinfo.kernelName, sysinfo.kernelVersion);
    Sys::println("Hostname: {}", sysinfo.hostname);

    Sys::println("Memory: {} / {}", meminfo.physicalUsed, meminfo.physicalTotal);
    Sys::println("Swap: {} / {}", meminfo.swapUsed, meminfo.swapTotal);

    for (auto &cpu : cpusinfo) {
        Sys::println("CPU: {} {} {} MHz", cpu.name, cpu.brand, cpu.freq);
    }

    return Cli::SUCCESS;
}
