module;

#include <karm-sys/dir.h>

export module Hideo.Files:dialogs;

import Karm.Kira;
import Karm.Ui;

import :widgets;

namespace Hideo::Files {

export Ui::Child openDialog(Ui::Send<Mime::Url> onFile) {
    return Ui::reducer<Model>(
        {"location://home"_url},
        [onFile](State const& s) {
            auto maybeDir = Sys::Dir::open(s.currentUrl());

            return Kr::dialogContent({
                Kr::dialogTitleBar("Open File…"s),
                toolbar(s),
                (maybeDir
                     ? directoryListing(s, maybeDir.unwrap())
                     : alert(
                           s,
                           "Can't access this location"s,
                           Io::toStr(maybeDir.none())
                       )
                ) | Ui::pinSize({400, 260}),
                Kr::separator(),
                Kr::dialogFooter({
                    Ui::grow(NONE),
                    Kr::dialogCancel(),
                    Kr::dialogAction(
                        [&, onFile](auto& n) {
                            onFile(n, s.currentUrl());
                        },
                        "Open"s
                    ),
                }),
            });
        }
    );
}

export Ui::Child saveDialog(Ui::Send<Mime::Url> onFile) {
    return Ui::reducer<Model>(
        {"location://home"_url},
        [onFile](State const& s) {
            auto maybeDir = Sys::Dir::open(s.currentUrl());

            return Kr::dialogContent({
                Kr::dialogTitleBar("Save As…"s),
                toolbar(s),
                (maybeDir
                     ? directoryListing(s, maybeDir.unwrap())
                     : alert(
                           s,
                           "Can't access this location"s,
                           Io::toStr(maybeDir.none())
                       )
                ) | Ui::pinSize({400, 260}),
                Kr::separator(),
                Kr::dialogFooter({
                    Ui::grow(NONE),
                    Kr::dialogCancel(),
                    Kr::dialogAction(
                        [&, onFile](auto& n) {
                            onFile(n, s.currentUrl());
                        },
                        "Save"s
                    ),
                }),
            });
        }
    );
}

export Ui::Child directoryDialog(Ui::Send<Mime::Url> onFile) {
    return Ui::reducer<Model>(
        {"location://home"_url},
        [onFile](auto const& d) {
            auto maybeDir = Sys::Dir::open(d.currentUrl());

            return Kr::dialogContent({
                Kr::dialogTitleBar("Select Directory…"s),
                toolbar(d),
                (maybeDir
                     ? directoryListing(d, maybeDir.unwrap())
                     : alert(
                           d,
                           "Can't access this location"s,
                           Io::toStr(maybeDir.none())
                       )
                ) | Ui::pinSize({400, 260}),
                Kr::separator(),
                Kr::dialogFooter({
                    Ui::grow(NONE),
                    Kr::dialogCancel(),
                    Kr::dialogAction(
                        [&, onFile](auto& n) {
                            onFile(n, d.currentUrl());
                        },
                        "Select"s
                    ),
                }),
            });
        }
    );
}

} // namespace Hideo::Files
