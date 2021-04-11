#include <ctype.h>

#include "kernel/graphics/EarlyConsole.h"
#include "kernel/graphics/Font.h"
#include "kernel/graphics/Graphics.h"

int _console_x = 0;
int _console_y = 0;
bool _console_escaped = false;
bool _console_enabled = true;

int early_console_width()
{
    return graphic_framebuffer_width() / font_width();
}

int early_console_height()
{
    return graphic_framebuffer_height() / font_height();
}

void early_console_disable()
{
    _console_x = 0;
    _console_y = 0;
    _console_enabled = false;
}

void early_console_enable()
{
    if (!_console_enabled)
    {
        _console_x = 0;
        _console_y = 0;
        _console_enabled = true;

        for (int x = 0; x <= early_console_width(); x++)
        {
            for (int y = 0; y <= early_console_height(); y++)
            {
                font_draw((uint8_t)0, x * font_width(), y * font_height());
            }
        }
    }
}

size_t early_console_write(const void *buffer, size_t size)
{
    if (!_console_enabled)
    {
        return size;
    }

    if (!graphic_has_framebuffer())
    {
        return size;
    }

    uint8_t *cp = (uint8_t *)buffer;

    for (size_t i = 0; i < size; i++)
    {
        if (_console_escaped)
        {
            if (isalpha(cp[i]))
            {
                _console_escaped = false;
            }
        }
        else
        {
            if (cp[i] == '\n')
            {
                _console_x = 0;
                _console_y = (_console_y + 1) % early_console_height();
            }
            if (cp[i] == '\t')
            {
                int old_console_x = _console_x;
                _console_x = (_console_x + 4) % early_console_width();

                if (old_console_x > _console_x)
                {
                    _console_y = (_console_y + 1) % early_console_height();
                }
            }
            else if (cp[i] == '\e')
            {
                _console_escaped = true;
            }
            else
            {
                font_draw(cp[i], _console_x * font_width(), _console_y * font_height());

                int old_console_x = _console_x;
                _console_x = (_console_x + 1) % early_console_width();

                if (old_console_x > _console_x)
                {
                    _console_y = (_console_y + 1) % early_console_height();
                }
            }
        }
    }

    return size;
}
