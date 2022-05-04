#pragma once

namespace Karm::Ui {

struct Hook {
    virtual ~Hook() = default;

    virtual void onMount() = 0;
    virtual void onUnmount() = 0;
};

} // namespace Karm::Ui
