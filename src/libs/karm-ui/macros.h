#pragma once

namespace Karm::Ui {

#define slot$(EXPR)      \
    Karm::Ui::Slot {     \
        [=] {            \
            return EXPR; \
        }                \
    }

#define slots$(...)               \
    Karm::Ui::Slots {             \
        [=] -> Ui::Children {     \
            return {__VA_ARGS__}; \
        }                         \
    }

} // namespace Karm::Ui
