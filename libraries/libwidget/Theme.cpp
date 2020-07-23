#include <libjson/Json.h>
#include <libsystem/core/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/utils/NumberParser.h>
#include <libwidget/Theme.h>

static bool _theme_is_dark = true;

static Color _theme_default_colors[__THEME_COLOR_COUNT] = {
    [THEME_BORDER] = THEME_DEFAULT_BORDER,
    [THEME_BACKGROUND] = THEME_DEFAULT_BACKGROUND,
    [THEME_MIDDLEGROUND] = THEME_DEFAULT_MIDDLEGROUND,
    [THEME_FOREGROUND] = THEME_DEFAULT_FOREGROUND,
    [THEME_FOREGROUND_INACTIVE] = THEME_DEFAULT_FOREGROUND_INACTIVE,
    [THEME_FOREGROUND_DISABLED] = THEME_DEFAULT_FOREGROUND_DISABLED,
    [THEME_SELECTION] = THEME_DEFAULT_SELECTION,
    [THEME_SELECTION_INACTIVE] = THEME_DEFAULT_SELECTION_INACTIVE,
    [THEME_ACCENT] = THEME_DEFAULT_ACCENT,
    [THEME_ACCENT_INACTIVE] = THEME_DEFAULT_ACCENT_INACTIVE,
    [THEME_ANSI_CURSOR] = THEME_DEFAULT_ANSI_CURSOR,
    [THEME_ANSI_BACKGROUND] = THEME_DEFAULT_ANSI_BACKGROUND,
    [THEME_ANSI_FOREGROUND] = THEME_DEFAULT_ANSI_FOREGROUND,
    [THEME_ANSI_BLACK] = THEME_DEFAULT_ANSI_BLACK,
    [THEME_ANSI_RED] = THEME_DEFAULT_ANSI_RED,
    [THEME_ANSI_GREEN] = THEME_DEFAULT_ANSI_GREEN,
    [THEME_ANSI_YELLOW] = THEME_DEFAULT_ANSI_YELLOW,
    [THEME_ANSI_BLUE] = THEME_DEFAULT_ANSI_BLUE,
    [THEME_ANSI_MAGENTA] = THEME_DEFAULT_ANSI_MAGENTA,
    [THEME_ANSI_CYAN] = THEME_DEFAULT_ANSI_CYAN,
    [THEME_ANSI_WHITE] = THEME_DEFAULT_ANSI_WHITE,
    [THEME_ANSI_BRIGHT_BLACK] = THEME_DEFAULT_ANSI_BRIGHT_BLACK,
    [THEME_ANSI_BRIGHT_RED] = THEME_DEFAULT_ANSI_BRIGHT_RED,
    [THEME_ANSI_BRIGHT_GREEN] = THEME_DEFAULT_ANSI_BRIGHT_GREEN,
    [THEME_ANSI_BRIGHT_YELLOW] = THEME_DEFAULT_ANSI_BRIGHT_YELLOW,
    [THEME_ANSI_BRIGHT_BLUE] = THEME_DEFAULT_ANSI_BRIGHT_BLUE,
    [THEME_ANSI_BRIGHT_MAGENTA] = THEME_DEFAULT_ANSI_BRIGHT_MAGENTA,
    [THEME_ANSI_BRIGHT_CYAN] = THEME_DEFAULT_ANSI_BRIGHT_CYAN,
    [THEME_ANSI_BRIGHT_WHITE] = THEME_DEFAULT_ANSI_BRIGHT_WHITE,
};

static Color _theme_colors[__THEME_COLOR_COUNT] = {
    [THEME_BORDER] = THEME_DEFAULT_BORDER,
    [THEME_BACKGROUND] = THEME_DEFAULT_BACKGROUND,
    [THEME_MIDDLEGROUND] = THEME_DEFAULT_MIDDLEGROUND,
    [THEME_FOREGROUND] = THEME_DEFAULT_FOREGROUND,
    [THEME_FOREGROUND_INACTIVE] = THEME_DEFAULT_FOREGROUND_INACTIVE,
    [THEME_FOREGROUND_DISABLED] = THEME_DEFAULT_FOREGROUND_DISABLED,
    [THEME_SELECTION] = THEME_DEFAULT_SELECTION,
    [THEME_SELECTION_INACTIVE] = THEME_DEFAULT_SELECTION_INACTIVE,
    [THEME_ACCENT] = THEME_DEFAULT_ACCENT,
    [THEME_ACCENT_INACTIVE] = THEME_DEFAULT_ACCENT_INACTIVE,
    [THEME_ANSI_CURSOR] = THEME_DEFAULT_ANSI_CURSOR,
    [THEME_ANSI_BACKGROUND] = THEME_DEFAULT_ANSI_BACKGROUND,
    [THEME_ANSI_FOREGROUND] = THEME_DEFAULT_ANSI_FOREGROUND,
    [THEME_ANSI_BLACK] = THEME_DEFAULT_ANSI_BLACK,
    [THEME_ANSI_RED] = THEME_DEFAULT_ANSI_RED,
    [THEME_ANSI_GREEN] = THEME_DEFAULT_ANSI_GREEN,
    [THEME_ANSI_YELLOW] = THEME_DEFAULT_ANSI_YELLOW,
    [THEME_ANSI_BLUE] = THEME_DEFAULT_ANSI_BLUE,
    [THEME_ANSI_MAGENTA] = THEME_DEFAULT_ANSI_MAGENTA,
    [THEME_ANSI_CYAN] = THEME_DEFAULT_ANSI_CYAN,
    [THEME_ANSI_WHITE] = THEME_DEFAULT_ANSI_WHITE,
    [THEME_ANSI_BRIGHT_BLACK] = THEME_DEFAULT_ANSI_BRIGHT_BLACK,
    [THEME_ANSI_BRIGHT_RED] = THEME_DEFAULT_ANSI_BRIGHT_RED,
    [THEME_ANSI_BRIGHT_GREEN] = THEME_DEFAULT_ANSI_BRIGHT_GREEN,
    [THEME_ANSI_BRIGHT_YELLOW] = THEME_DEFAULT_ANSI_BRIGHT_YELLOW,
    [THEME_ANSI_BRIGHT_BLUE] = THEME_DEFAULT_ANSI_BRIGHT_BLUE,
    [THEME_ANSI_BRIGHT_MAGENTA] = THEME_DEFAULT_ANSI_BRIGHT_MAGENTA,
    [THEME_ANSI_BRIGHT_CYAN] = THEME_DEFAULT_ANSI_BRIGHT_CYAN,
    [THEME_ANSI_BRIGHT_WHITE] = THEME_DEFAULT_ANSI_BRIGHT_WHITE,
};

static const char *_theme_colors_names[__THEME_COLOR_COUNT] = {
    [THEME_BORDER] = "border",
    [THEME_BACKGROUND] = "background",
    [THEME_MIDDLEGROUND] = "middleground",
    [THEME_FOREGROUND] = "foreground",
    [THEME_FOREGROUND_INACTIVE] = "foreground-inactive",
    [THEME_FOREGROUND_DISABLED] = "foreground-disabled",
    [THEME_SELECTION] = "selection",
    [THEME_SELECTION_INACTIVE] = "selection-inactive",
    [THEME_ACCENT] = "accent",
    [THEME_ACCENT_INACTIVE] = "accent-inactive",
    [THEME_ANSI_CURSOR] = "ansi-cursor",
    [THEME_ANSI_BACKGROUND] = "ansi-background",
    [THEME_ANSI_FOREGROUND] = "ansi-foreground",
    [THEME_ANSI_BLACK] = "ansi-black",
    [THEME_ANSI_RED] = "ansi-red",
    [THEME_ANSI_GREEN] = "ansi-green",
    [THEME_ANSI_YELLOW] = "ansi-yellow",
    [THEME_ANSI_BLUE] = "ansi-blue",
    [THEME_ANSI_MAGENTA] = "ansi-magenta",
    [THEME_ANSI_CYAN] = "ansi-cyan",
    [THEME_ANSI_WHITE] = "ansi-white",
    [THEME_ANSI_BRIGHT_BLACK] = "ansi-bright-black",
    [THEME_ANSI_BRIGHT_RED] = "ansi-bright-red",
    [THEME_ANSI_BRIGHT_GREEN] = "ansi-bright-green",
    [THEME_ANSI_BRIGHT_YELLOW] = "ansi-bright-yellow",
    [THEME_ANSI_BRIGHT_BLUE] = "ansi-bright-blue",
    [THEME_ANSI_BRIGHT_MAGENTA] = "ansi-bright-magenta",
    [THEME_ANSI_BRIGHT_CYAN] = "ansi-bright-cyan",
    [THEME_ANSI_BRIGHT_WHITE] = "ansi-bright-white",
};

bool theme_is_dark(void)
{
    return _theme_is_dark;
}

Color theme_parse_color(const char *text)
{
    if (text[0] == '#')
    {
        if (strlen(&text[1]) == 3)
        {
            unsigned int packed = 0;
            if (parse_uint(PARSER_HEXADECIMAL, &text[1], 3, &packed))
            {
                uint8_t red = packed >> 8 & 0xf;
                uint8_t green = packed >> 4 & 0xf;
                uint8_t blue = packed & 0xf;

                return COLOR_RGBA(
                    red << 4 | red,
                    green << 4 | green,
                    blue << 4 | blue,
                    0xff);
            }
        }
        else if (strlen(&text[1]) == 4)
        {
            unsigned int packed = 0;
            if (parse_uint(PARSER_HEXADECIMAL, &text[1], 4, &packed))
            {
                uint8_t red = packed >> 12 & 0xf;
                uint8_t green = packed >> 8 & 0xf;
                uint8_t blue = packed >> 4 & 0xf;
                uint8_t alpha = packed & 0xf;

                return COLOR_RGBA(
                    red << 4 | red,
                    green << 4 | green,
                    blue << 4 | blue,
                    alpha << 4 | alpha);
            }
        }
        else if (strlen(&text[1]) == 6)
        {
            unsigned int packed = 0;
            if (parse_uint(PARSER_HEXADECIMAL, &text[1], 6, &packed))
            {
                return COLOR(packed);
            }
        }
        else if (strlen(&text[1]) == 8)
        {
            unsigned int packed = 0;
            if (parse_uint(PARSER_HEXADECIMAL, &text[1], 8, &packed))
            {
                Color color = COLOR(packed >> 8);
                color.A = packed & 0xff;
                return color;
            }
        }
        return COLOR_GREEN;
    }

    logger_warn("Failled to parse %c '%s' %d", text[0], text, strlen(text));

    return COLOR_MAGENTA;
}

void theme_load(const char *path)
{
    logger_info("Loading theme from '%s'", path);
    JsonValue *root = json_parse_file(path);

    if (!json_is(root, JSON_OBJECT) || !json_object_has(root, "colors"))
    {
        json_destroy(root);
        memcpy(_theme_colors, _theme_default_colors, sizeof(_theme_default_colors));
        return;
    }

    JsonValue *colors = json_object_get(root, "colors");

    if (json_object_has(root, "dark"))
    {
        JsonValue *is_dark = json_object_get(root, "dark");

        _theme_is_dark = json_is(is_dark, JSON_TRUE);
    }
    else
    {
        _theme_is_dark = false;
    }

    if (!json_is(colors, JSON_OBJECT))
    {
        json_destroy(root);
        memcpy(_theme_colors, _theme_default_colors, sizeof(_theme_default_colors));
        return;
    }

    for (int i = 0; i < __THEME_COLOR_COUNT; i++)
    {
        if (json_object_has(colors, _theme_colors_names[i]))
        {
            JsonValue *color = json_object_get(colors, _theme_colors_names[i]);

            if (json_is(color, JSON_STRING))
            {
                _theme_colors[i] = theme_parse_color(json_string_value(color));
            }
        }
    }

    json_destroy(root);
}

Color theme_get_color(ThemeColorRole role)
{
    return _theme_colors[role];
}
