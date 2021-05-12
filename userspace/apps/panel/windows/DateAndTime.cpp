#include <libwidget/Screen.h>

#include "panel/Constants.h"
#include "panel/windows/DateAndTime.h"

using namespace Widget;

namespace Panel
{

DateAndTime::DateAndTime()
    : Window(WINDOW_ALWAYS_FOCUSED | WINDOW_AUTO_CLOSE | WINDOW_ACRYLIC)
{
    bound(Math::Recti{320, 256}
              .centered_within(Screen::bound())
              .with_y(PANEL_WINDOW_HEIGHT));

    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);
}

} // namespace Panel