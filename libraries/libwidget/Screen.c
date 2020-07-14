#include <libwidget/Screen.h>

static Rectangle _screen_bound;

Rectangle screen_get_bound(void)
{
    return _screen_bound;
}

void screen_set_bound(Rectangle rectangle)
{
    _screen_bound = rectangle;
}
