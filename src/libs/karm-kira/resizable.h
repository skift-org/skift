#pragma once

#include <karm-ui/drag.h>
#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

enum struct ResizeHandle {
    TOP,
    START,
    BOTTOM,
    END
};

Ui::Child resizable(Ui::Child child, Math::Vec2i size, Ui::OnChange<Math::Vec2i> onChange);

Ui::Child resizable(Ui::Child child, ResizeHandle handlePosition, Math::Vec2i size, Ui::OnChange<Math::Vec2i> onChange);

static inline auto resizable(Math::Vec2i size, Ui::OnChange<Math::Vec2i> onChange) {
    return [size, onChange = std::move(onChange)](Ui::Child child) mutable -> Ui::Child {
        return resizable(child, size, std::move(onChange));
    };
}

static inline auto resizable(ResizeHandle handlePosition, Math::Vec2i size, Ui::OnChange<Math::Vec2i> onChange) {
    return [handlePosition, size, onChange = std::move(onChange)](Ui::Child child) mutable -> Ui::Child {
        return resizable(child, handlePosition, size, std::move(onChange));
    };
}

} // namespace Karm::Kira
