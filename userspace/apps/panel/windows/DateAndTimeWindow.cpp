#include <libwidget/Screen.h>

#include "panel/windows/DateAndTimeWindow.h"
#include "panel/windows/PanelWindow.h"

namespace panel
{

DateAndTimeWindow::DateAndTimeWindow()
    : Window(WINDOW_ALWAYS_FOCUSED | WINDOW_AUTO_CLOSE | WINDOW_ACRYLIC)
{
    title("Date And Time");
    bound(Math::Recti{320, 256}.centered_within(Widget::Screen::bound()).with_y(PanelWindow::HEIGHT));
    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);
}

} // namespace panel