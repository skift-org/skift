#include <karm-kira/card.h>
#include <karm-kira/row.h>
#include <karm-kira/scaffold.h>
#include <karm-sys/info.h>
#include <karm-ui/error.h>
#include <mdi/account.h>
#include <mdi/cpu-64-bit.h>
#include <mdi/information-outline.h>
#include <mdi/memory.h>

#include "app.h"

namespace Hideo::Settings {

Ui::Child userInfos() {
    return Kr::treeRow(
        [&] {
            return Ui::icon(Mdi::ACCOUNT);
        },
        "User"s,
        NONE,
        Ui::Slot{[&] {
            return Ui::errorScope("user informations", [&] -> Res<Ui::Child> {
                auto userinfo = try$(Sys::userinfo());
                return Ok(Ui::vflow(
                    Kr::rowContent(NONE, "Username"s, NONE, Ui::text(userinfo.name)),
                    Kr::rowContent(NONE, "Home"s, NONE, Ui::text(userinfo.home.str())),
                    Kr::rowContent(NONE, "Shell"s, NONE, Ui::text(userinfo.shell.str()))
                ));
            });
        }}
    );
}

Ui::Child sysInfos() {
    return Kr::treeRow(
        [&] {
            return Ui::icon(Mdi::INFORMATION_OUTLINE);
        },
        "System"s,
        NONE,
        Ui::Slot{[&] {
            return Ui::errorScope("system informations", [] -> Res<Ui::Child> {
                auto sysinfo = try$(Sys::sysinfo());
                return Ok(Ui::vflow(
                    Kr::rowContent(NONE, "System"s, NONE, Ui::text(sysinfo.sysName)),
                    Kr::rowContent(NONE, "System Version"s, NONE, Ui::text(sysinfo.sysVersion)),
                    Kr::rowContent(NONE, "Kernel"s, NONE, Ui::text(sysinfo.kernelName)),
                    Kr::rowContent(NONE, "Kernel Version"s, NONE, Ui::text(sysinfo.kernelVersion))
                ));
            });
        }}
    );
}

Ui::Child memInfos() {
    return Kr::treeRow(
        [&] {
            return Ui::icon(Mdi::MEMORY);
        },
        "Memory"s,
        NONE,
        Ui::Slot{[&] {
            return Ui::errorScope("memory informations"s, [&] -> Res<Ui::Child> {
                auto meminfo = try$(Sys::meminfo());
                return Ok(vflow(
                    Kr::rowContent(NONE, "Physical"s, NONE, Ui::text("{}bytes", meminfo.physicalUsed)),
                    Kr::rowContent(NONE, "Swap"s, NONE, Ui::text("{}bytes", meminfo.swapUsed))
                ));
            });
        }}
    );
}

Ui::Child cpuInfos() {
    return Kr::treeRow(
        [&] {
            return Ui::icon(Mdi::CPU_64_BIT);
        },
        "CPU"s,
        NONE,
        Ui::Slot{[&] {
            return Ui::errorScope("CPU informations", [&] -> Res<Ui::Child> {
                auto cpusinfo = try$(Sys::cpusinfo());

                Ui::Children children;

                for (auto& cpu : cpusinfo) {
                    children.pushBack(Kr::rowContent(
                        NONE,
                        "CPU"s,
                        NONE,
                        Ui::text("{} {} {} MHz", cpu.name, cpu.brand, cpu.freq)
                    ));
                }

                return Ok(Ui::vflow(children));
            });
        }}
    );
}

Ui::Child pageAbout(State const&) {
    return Ui::vflow(
               8,
               Kr::titleRow("About"s),
               userInfos() | Kr::card(),
               sysInfos() | Kr::card(),
               memInfos() | Kr::card(),
               cpuInfos() | Kr::card()
           ) |
           pagescaffold;
}

} // namespace Hideo::Settings
