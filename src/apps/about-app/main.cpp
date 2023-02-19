#include <karm-main/main.h>
#include <karm-sys/info.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

Ui::Child errorScope(auto callback) {
    Res<Ui::Child> child = callback();

    if (child) {
        return child.unwrap();
    }

    return Ui::box(
        {
            .padding = 12,
            .borderRadius = 6,
            .borderWidth = 1,
            .borderPaint = Gfx::RED700,
            .backgroundPaint = Gfx::RED900,
            .foregroundPaint = Gfx::RED100,
        },
        Ui::hflow(
            6,
            Ui::center(
                Ui::icon(
                    Media::Icons::ALERT_DECAGRAM_OUTLINE, 26)),
            Ui::vflow(
                4,
                Ui::text(Ui::TextStyle::titleMedium().withColor(Gfx::WHITE), "Something went wrong"),
                Ui::text(child.none().msg()))));
}

Ui::Child userInfos() {
    return Ui::treeRow(
        Ui::icon(Media::Icons::ACCOUNT),
        "User",
        NONE,
        errorScope([&] -> Res<Ui::Child> {
            auto userinfo = try$(Sys::userinfo());
            return Ok(Ui::vflow(
                Ui::row(NONE, "Username", NONE, Ui::text(userinfo.name)),
                Ui::row(NONE, "Home", NONE, Ui::text(userinfo.home)),
                Ui::row(NONE, "Shell", NONE, Ui::text(userinfo.shell))));
        }));
}

Ui::Child sysInfos() {
    return Ui::treeRow(
        Ui::icon(Media::Icons::INFORMATION_OUTLINE),
        "System",
        NONE,
        errorScope([&] -> Res<Ui::Child> {
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
        Ui::icon(Media::Icons::MEMORY),
        "Memory",
        NONE,
        errorScope([&] -> Res<Ui::Child> {
            auto meminfo = try$(Sys::meminfo());
            return Ok(vflow(
                Ui::row(NONE, "Physical", NONE, Ui::text("{}bytes", meminfo.physicalUsed)),
                Ui::row(NONE, "Swap", NONE, Ui::text("{}bytes", meminfo.swapUsed))));
        }));
}

Ui::Child cpuInfos() {
    return Ui::treeRow(
        Ui::icon(Media::Icons::CPU_64_BIT),
        "CPU",
        NONE,
        errorScope([&] -> Res<Ui::Child> {
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

Ui::Child details() {
    return Ui::grow(
        Ui::vscroll(
            Ui::card(
                Ui::vflow(
                    userInfos(),
                    Ui::separator(),
                    sysInfos(),
                    Ui::separator(),
                    memInfos(),
                    Ui::separator(),
                    cpuInfos()))));
}

Res<> entryPoint(CliArgs args) {
    auto logo = Ui::box(
        {
            .padding = 32,
            .backgroundPaint = Gfx::WHITE,
            .foregroundPaint = Gfx::BLACK,
        },
        Ui::bound(Ui::center(Ui::icon(Media::Icons::SNOWFLAKE, 64))));

    auto licenseBtn = Ui::button(
        NONE,
        Ui::ButtonStyle::subtle().withRadius(999),
        Media::Icons::LICENSE,
        "LICENSE");

    auto closeBtn = Ui::button(
        [](Ui::Node &n) {
            Events::ExitEvent e{Ok()};
            n.bubble(e);
        },
        Ui::ButtonStyle::primary(),
        "OK");

    auto content = Ui::spacing(
        16,
        Ui::vflow(
            8,
            Ui::hflow(8,
                      Ui::text(Ui::TextStyle::titleLarge(), "skiftOS"),
                      Ui::center(Ui::badge(Ui::BadgeStyle::INFO, "v0.1.0"))),
            Ui::empty(),
            Ui::text("Copyright © 2018-2022"),
            Ui::text("SMNX & contributors."),
            details(),
            Ui::hflow(8, licenseBtn, Ui::grow(), closeBtn)));

    auto titlebar = Ui::titlebar(
        Media::Icons::INFORMATION,
        "About",
        Ui::TitlebarStyle::DIALOG);

    auto wrapper =
        Ui::vflow(titlebar, logo, Ui::grow(content));

    auto layout = Ui::dragRegion(
        Ui::pinSize(
            {400, 550},
            wrapper));

    return Ui::runApp(args, Ui::dialogLayer(layout));
}
