#include <karm-sys/info.h>
#include <karm-ui/error.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>

#include "app.h"

namespace Settings {

Ui::Child userInfos() {
    return Ui::treeRow(
        Ui::icon(Mdi::ACCOUNT),
        "User",
        NONE,
        Ui::errorScope("user informations", [&] -> Res<Ui::Child> {
            auto userinfo = try$(Sys::userinfo());
            return Ok(Ui::vflow(
                Ui::row(NONE, "Username", NONE, Ui::text(userinfo.name)),
                Ui::row(NONE, "Home", NONE, Ui::text(try$(userinfo.home.str()))),
                Ui::row(NONE, "Shell", NONE, Ui::text(try$(userinfo.shell.str())))));
        }));
}

Ui::Child sysInfos() {
    return Ui::treeRow(
        Ui::icon(Mdi::INFORMATION_OUTLINE),
        "System",
        NONE,
        Ui::errorScope("system informations", [&] -> Res<Ui::Child> {
            auto sysinfo = try$(Sys::sysinfo());
            return Ok(Ui::vflow(
                Ui::row(NONE, "System", NONE, Ui::text(sysinfo.sysName)),
                Ui::row(NONE, "System Version", NONE, Ui::text(sysinfo.sysVersion)),
                Ui::row(NONE, "Kernel", NONE, Ui::text(sysinfo.kernelName)),
                Ui::row(NONE, "Kernel Version", NONE, Ui::text(sysinfo.kernelVersion))));
        }));
}

Ui::Child memInfos() {
    return Ui::treeRow(
        Ui::icon(Mdi::MEMORY),
        "Memory",
        NONE,
        Ui::errorScope("memory informations", [&] -> Res<Ui::Child> {
            auto meminfo = try$(Sys::meminfo());
            return Ok(vflow(
                Ui::row(NONE, "Physical", NONE, Ui::text("{}bytes", meminfo.physicalUsed)),
                Ui::row(NONE, "Swap", NONE, Ui::text("{}bytes", meminfo.swapUsed))));
        }));
}

Ui::Child cpuInfos() {
    return Ui::treeRow(
        Ui::icon(Mdi::CPU_64_BIT),
        "CPU",
        NONE,
        Ui::errorScope("CPU informations", [&] -> Res<Ui::Child> {
            auto cpusinfo = try$(Sys::cpusinfo());

            Ui::Children children;

            for (auto &cpu : cpusinfo) {
                children.pushBack(Ui::row(
                    NONE,
                    "CPU",
                    NONE,
                    Ui::text("{} {} {} MHz", cpu.name, cpu.brand, cpu.freq)));
            }

            return Ok(Ui::vflow(children));
        }));
}

Ui::Child pageAbout(State const &) {
    return Ui::vflow(
               8,
               Ui::titleRow("About"),
               userInfos() | Ui::card(),
               sysInfos() | Ui::card(),
               memInfos() | Ui::card(),
               cpuInfos() | Ui::card()) |
           pageScafold;
}

} // namespace Settings
