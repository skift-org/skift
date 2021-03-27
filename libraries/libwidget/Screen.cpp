#include <libwidget/Screen.h>

namespace Widget::Screen
{

static Recti _bound;

Recti bound()
{
    return _bound;
}

void bound(Recti bound)
{
    _bound = bound;
}

} // namespace Widget::Screen
