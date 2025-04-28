module;

#include <karm-gfx/colors.h>
#include <karm-sys/info.h>

export module Hideo.Settings:page_about;

import Mdi;
import Karm.Kira;
import Karm.Ui;
import :model;
import :common;

namespace Hideo::Settings {

Ui::Child errorScope(Str what, auto callback) {
    Res<Ui::Child> child = callback();

    if (child) {
        return child.unwrap();
    }

    return Ui::box(
        {
            .margin = 12,
            .padding = 4,
            .borderRadii = 6,
            .borderWidth = 1,
            .borderFill = Gfx::RED700,
            .backgroundFill = Gfx::RED900,
            .foregroundFill = Gfx::RED100,
        },
        Ui::hflow(
            6,
            Ui::icon(Mdi::ALERT_OCTAGON_OUTLINE, 24) | Ui::center(),
            Ui::vflow(
                Ui::text(Ui::TextStyles::labelMedium().withColor(Gfx::WHITE), "Can't display {}", what),
                Ui::text(Ui::TextStyles::bodySmall(), Io::toSentenceCase(child.none().msg()).unwrap())
            )
        )
    );
}

Ui::Child userInfos() {
    return Kr::treeRow(
        [&] {
            return Ui::icon(Mdi::ACCOUNT);
        },
        "User"s,
        NONE,
        Ui::Slot{[&] {
            return errorScope("user informations", [&] -> Res<Ui::Child> {
                auto userinfo = try$(Sys::userinfo());
                return Ok(Ui::vflow(Kr::rowContent(NONE, "Username"s, NONE, Ui::text(userinfo.name)), Kr::rowContent(NONE, "Home"s, NONE, Ui::text(userinfo.home.str())), Kr::rowContent(NONE, "Shell"s, NONE, Ui::text(userinfo.shell.str()))));
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
            return errorScope("system informations", [] -> Res<Ui::Child> {
                auto sysinfo = try$(Sys::sysinfo());
                return Ok(Ui::vflow(Kr::rowContent(NONE, "System"s, NONE, Ui::text(sysinfo.sysName)), Kr::rowContent(NONE, "System Version"s, NONE, Ui::text(sysinfo.sysVersion)), Kr::rowContent(NONE, "Kernel"s, NONE, Ui::text(sysinfo.kernelName)), Kr::rowContent(NONE, "Kernel Version"s, NONE, Ui::text(sysinfo.kernelVersion))));
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
            return errorScope("memory informations"s, [&] -> Res<Ui::Child> {
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
            return errorScope("CPU informations", [&] -> Res<Ui::Child> {
                auto cpusinfo = try$(Sys::cpusinfo());

                Ui::Children children;

                for (auto& cpu : cpusinfo) {
                    children.pushBack(Kr::rowContent(NONE, "CPU"s, NONE, Ui::text("{} {} {} MHz", cpu.name, cpu.brand, cpu.freq)));
                }

                return Ok(Ui::vflow(children));
            });
        }}
    );
}

export Ui::Child pageAbout(State const&) {
    return Ui::vflow(
               8,
               Kr::titleRow("About"s),
               userInfos() | Kr::card(),
               sysInfos() | Kr::card(),
               memInfos() | Kr::card(),
               cpuInfos() | Kr::card()
           ) |
           pageScaffold;
}

} // namespace Hideo::Settings
