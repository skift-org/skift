#include <abi/Paths.h>
#include <abi/TextMode.h>

#include "terminal/TextmodeTerminal.h"

static uint8_t textmode_colors[__TERMINAL_COLOR_COUNT] = {
    [TERMINAL_COLOR_BLACK] = TEXTMODE_COLOR_BLACK,
    [TERMINAL_COLOR_RED] = TEXTMODE_COLOR_RED,
    [TERMINAL_COLOR_GREEN] = TEXTMODE_COLOR_GREEN,
    [TERMINAL_COLOR_YELLOW] = TEXTMODE_COLOR_BROWN,
    [TERMINAL_COLOR_BLUE] = TEXTMODE_COLOR_BLUE,
    [TERMINAL_COLOR_MAGENTA] = TEXTMODE_COLOR_MAGENTA,
    [TERMINAL_COLOR_CYAN] = TEXTMODE_COLOR_CYAN,
    [TERMINAL_COLOR_GREY] = TEXTMODE_COLOR_LIGHT_GRAY,

    [TERMINAL_COLOR_BRIGHT_BLACK] = TEXTMODE_COLOR_GRAY,
    [TERMINAL_COLOR_BRIGHT_RED] = TEXTMODE_COLOR_LIGHT_RED,
    [TERMINAL_COLOR_BRIGHT_GREEN] = TEXTMODE_COLOR_LIGHT_GREEN,
    [TERMINAL_COLOR_BRIGHT_YELLOW] = TEXTMODE_COLOR_LIGHT_YELLOW,
    [TERMINAL_COLOR_BRIGHT_BLUE] = TEXTMODE_COLOR_LIGHT_BLUE,
    [TERMINAL_COLOR_BRIGHT_MAGENTA] = TEXTMODE_COLOR_LIGHT_MAGENTA,
    [TERMINAL_COLOR_BRIGHT_CYAN] = TEXTMODE_COLOR_LIGHT_CYAN,
    [TERMINAL_COLOR_BRIGHT_GREY] = TEXTMODE_COLOR_WHITE,

    [TERMINAL_COLOR_DEFAULT_FOREGROUND] = TEXTMODE_COLOR_LIGHT_GRAY,
    [TERMINAL_COLOR_DEFAULT_BACKGROUND] = TEXTMODE_COLOR_BLACK,
};

void textmode_terminal_on_paint(Terminal *terminal, TextmodeTerminalRenderer *renderer, int x, int y, TerminalCell cell)
{
    if (cell.attributes.inverted)
    {
        renderer->buffer[x + y * terminal->width] = TEXTMODE_ENTRY(
            codepoint_to_cp437(cell.codepoint),
            textmode_colors[cell.attributes.background],
            textmode_colors[cell.attributes.foreground]);
    }
    else
    {
        renderer->buffer[x + y * terminal->width] = TEXTMODE_ENTRY(
            codepoint_to_cp437(cell.codepoint),
            textmode_colors[cell.attributes.foreground],
            textmode_colors[cell.attributes.background]);
    }
}

void textmode_terminal_repaint(Terminal *terminal, TextmodeTerminalRenderer *renderer)
{
    IOCallTextModeStateArgs args = {
        .width = -1,
        .height = -1,
        .cursor_x = terminal->cursor.x,
        .cursor_y = terminal->cursor.y,
    };

    stream_call(renderer->device, IOCALL_TEXTMODE_SET_STATE, &args);
    stream_write(renderer->device, renderer->buffer, terminal->width * terminal->height * sizeof(uint16_t));
}

void textmode_terminal_destroy(TextmodeTerminalRenderer *renderer)
{
    stream_close(renderer->device);
}

Terminal *textmode_terminal_create()
{
    Stream *device = stream_open(TEXTMODE_DEVICE_PATH, OPEN_WRITE);
    IOCallTextModeStateArgs args;

    if (handle_has_error(device))
    {
        stream_close(device);

        return nullptr;
    }

    if (stream_call(device, IOCALL_TEXTMODE_GET_STATE, &args) != SUCCESS)
    {
        stream_close(device);

        return nullptr;
    }

    TextmodeTerminalRenderer *renderer = __create(TextmodeTerminalRenderer);

    TERMINAL_RENDERER(renderer)->on_paint = (TerminalOnPaintCallback)textmode_terminal_on_paint;
    TERMINAL_RENDERER(renderer)->repaint = (TerminalRepaintCallback)textmode_terminal_repaint;
    TERMINAL_RENDERER(renderer)->destroy = (TerminalRendererDestroy)textmode_terminal_destroy;

    renderer->device = device;
    renderer->buffer = (uint16_t *)calloc(args.width * args.height, sizeof(uint16_t));

    return terminal_create(args.width, args.height, TERMINAL_RENDERER(renderer));
}
