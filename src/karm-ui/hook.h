#pragma once

namespace Karm::Ui {

struct Hook {
    virtual ~Hook() = default;

    virtual void on_mount() = 0;
    virtual void on_unmount() = 0;
};

} // namespace Karm::Ui
