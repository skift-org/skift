#include <hideo-base/row.h>
#include <hideo-base/scafold.h>
#include <karm-kira/card.h>
#include <karm-sys/info.h>
#include <karm-ui/error.h>

#include "app.h"

namespace Hideo::Settings {

Ui::Child userInfos() {
    return Hideo::treeRow(
        slot$(Ui::icon(Mdi::ACCOUNT)),
        "User"s,
        NONE,
        slots$(Ui::errorScope("user informations", [&] -> Res<Ui::Child> {
            auto userinfo = try$(Sys::userinfo());
            return Ok(Ui::vflow(
                Hideo::row(NONE, "Username"s, NONE, Ui::text(userinfo.name)),
                Hideo::row(NONE, "Home"s, NONE, Ui::text(userinfo.home.str())),
                Hideo::row(NONE, "Shell"s, NONE, Ui::text(userinfo.shell.str()))
            ));
        }))
    );
}

Ui::Child sysInfos() {
    return Hideo::treeRow(
        slot$(Ui::icon(Mdi::INFORMATION_OUTLINE)),
        "System"s,
        NONE,
        slot$(Ui::errorScope("system informations", [] -> Res<Ui::Child> {
            auto sysinfo = try$(Sys::sysinfo());
            return Ok(Ui::vflow(
                Hideo::row(NONE, "System"s, NONE, Ui::text(sysinfo.sysName)),
                Hideo::row(NONE, "System Version"s, NONE, Ui::text(sysinfo.sysVersion)),
                Hideo::row(NONE, "Kernel"s, NONE, Ui::text(sysinfo.kernelName)),
                Hideo::row(NONE, "Kernel Version"s, NONE, Ui::text(sysinfo.kernelVersion))
            ));
        }))
    );
}

Ui::Child memInfos() {
    return Hideo::treeRow(
        slot$(Ui::icon(Mdi::MEMORY)),
        "Memory"s,
        NONE,
        slot$(Ui::errorScope("memory informations"s, [&] -> Res<Ui::Child> {
            auto meminfo = try$(Sys::meminfo());
            return Ok(vflow(
                Hideo::row(NONE, "Physical"s, NONE, Ui::text("{}bytes", meminfo.physicalUsed)),
                Hideo::row(NONE, "Swap"s, NONE, Ui::text("{}bytes", meminfo.swapUsed))
            ));
        }))
    );
}

Ui::Child cpuInfos() {
    return Hideo::treeRow(
        slot$(Ui::icon(Mdi::CPU_64_BIT)),
        "CPU"s,
        NONE,
        slot$(Ui::errorScope("CPU informations", [&] -> Res<Ui::Child> {
            auto cpusinfo = try$(Sys::cpusinfo());

            Ui::Children children;

            for (auto &cpu : cpusinfo) {
                children.pushBack(Hideo::row(
                    NONE,
                    "CPU"s,
                    NONE,
                    Ui::text("{} {} {} MHz", cpu.name, cpu.brand, cpu.freq)
                ));
            }

            return Ok(Ui::vflow(children));
        }))
    );
}

Ui::Child pageAbout(State const &) {
    return Ui::vflow(
               8,
               Hideo::titleRow("About"s),
               userInfos() | Kr::card(),
               sysInfos() | Kr::card(),
               memInfos() | Kr::card(),
               cpuInfos() | Kr::card()
           ) |
           pageScafold;
}

} // namespace Hideo::Settings
