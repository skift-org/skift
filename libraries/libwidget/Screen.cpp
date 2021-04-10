#include <libwidget/Screen.h>

namespace Widget::Screen
{

static Math::Recti _bound;

Math::Recti bound()
{
    return _bound;
}

void bound(Math::Recti bound)
{
    _bound = bound;
}

} // namespace Widget::Screen
