#include <hideo-base/row.h>
#include <karm-sys/info.h>
#include <karm-ui/error.h>
#include <karm-ui/scafold.h>

#include "app.h"

namespace Hideo::Settings {

Ui::Child userInfos() {
    return Hideo::treeRow(
        slot$(Ui::icon(Mdi::ACCOUNT)),
        "User",
        NONE,
        slots$(Ui::errorScope("user informations", [&] -> Res<Ui::Child> {
            auto userinfo = try$(Sys::userinfo());
            return Ok(Ui::vflow(
                Hideo::row(NONE, "Username", NONE, Ui::text(userinfo.name)),
                Hideo::row(NONE, "Home", NONE, Ui::text(userinfo.home.str())),
                Hideo::row(NONE, "Shell", NONE, Ui::text(userinfo.shell.str()))));
        })));
}

Ui::Child sysInfos() {
    return Hideo::treeRow(
        slot$(Ui::icon(Mdi::INFORMATION_OUTLINE)),
        "System",
        NONE,
        slot$(Ui::errorScope("system informations", [] -> Res<Ui::Child> {
            auto sysinfo = try$(Sys::sysinfo());
            return Ok(Ui::vflow(
                Hideo::row(NONE, "System", NONE, Ui::text(sysinfo.sysName)),
                Hideo::row(NONE, "System Version", NONE, Ui::text(sysinfo.sysVersion)),
                Hideo::row(NONE, "Kernel", NONE, Ui::text(sysinfo.kernelName)),
                Hideo::row(NONE, "Kernel Version", NONE, Ui::text(sysinfo.kernelVersion))));
        })));
}

Ui::Child memInfos() {
    return Hideo::treeRow(
        slot$(Ui::icon(Mdi::MEMORY)),
        "Memory",
        NONE,
        slot$(Ui::errorScope("memory informations", [&] -> Res<Ui::Child> {
            auto meminfo = try$(Sys::meminfo());
            return Ok(vflow(
                Hideo::row(NONE, "Physical", NONE, Ui::text("{}bytes", meminfo.physicalUsed)),
                Hideo::row(NONE, "Swap", NONE, Ui::text("{}bytes", meminfo.swapUsed))));
        })));
}

Ui::Child cpuInfos() {
    return Hideo::treeRow(
        slot$(Ui::icon(Mdi::CPU_64_BIT)),
        "CPU",
        NONE,
        slot$(Ui::errorScope("CPU informations", [&] -> Res<Ui::Child> {
            auto cpusinfo = try$(Sys::cpusinfo());

            Ui::Children children;

            for (auto &cpu : cpusinfo) {
                children.pushBack(Hideo::row(
                    NONE,
                    "CPU",
                    NONE,
                    Ui::text("{} {} {} MHz", cpu.name, cpu.brand, cpu.freq)));
            }

            return Ok(Ui::vflow(children));
        })));
}

Ui::Child pageAbout(State const &) {
    return Ui::vflow(
               8,
               Hideo::titleRow("About"),
               userInfos() | Hideo::card(),
               sysInfos() | Hideo::card(),
               memInfos() | Hideo::card(),
               cpuInfos() | Hideo::card()) |
           pageScafold;
}

} // namespace Hideo::Settings
