#include <libsystem/Logger.h>
#include <libutils/json/Json.h>
#include <libutils/NumberParser.h>
#include <libwidget/Theme.h>
#include <string.h>

static bool _theme_is_dark = true;

static constexpr Color _theme_default_colors[__THEME_COLOR_COUNT] = {
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

bool theme_is_dark()
{
    return _theme_is_dark;
}

void theme_load(const char *path)
{
    logger_info("Loading theme from '%s'", path);

    auto root = json::parse_file(path);

    if (!root.has("colors"))
    {
        memcpy(_theme_colors, _theme_default_colors, sizeof(_theme_default_colors));
        return;
    }

    const auto &colors = root.get("colors");

    if (root.has("dark"))
    {
        _theme_is_dark = root.get("dark").is(json::TRUE);
    }
    else
    {
        _theme_is_dark = false;
    }

    for (int i = 0; i < __THEME_COLOR_COUNT; i++)
    {
        const auto &color = colors.get(_theme_colors_names[i]);

        if (color.is(json::STRING))
        {
            _theme_colors[i] = Color::parse(color.as_string());
        }
    }
}

Color theme_get_color(ThemeColorRole role)
{
    return _theme_colors[role];
}

void theme_set_color(ThemeColorRole role, Color color)
{
    _theme_colors[role] = color;
}
