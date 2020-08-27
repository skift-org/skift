#include <libwidget/Screen.h>

namespace Screen
{

static Rectangle _bound;

Rectangle bound()
{
    return _bound;
}

void bound(Rectangle bound)
{
    _bound = bound;
}

} // namespace Screen
