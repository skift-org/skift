/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include <math.h>

#include <skift/messaging.h>
#include <skift/process.h>
#include <skift/thread.h>
#include <skift/logger.h>
#include <skift/io.h>

#include <hideo.h>

void die(const char *msg)
{
    sk_log(LOG_FATAL, "Hideo die: %s", msg);
    sk_process_exit(-1);
}

/* --- Windows operation ---------------------------------------------------- */

void window_create()
{
}

void window_close()
{
}

// Painting

void window_repaint()
{
}

// Dragging

void window_begin_dragging()
{
}

void window_drag()
{
}

void window_end_dragging()
{
}

// Resizing

void window_begin_resizing()
{
}

void window_resize()
{
}

void window_end_resizing()
{
}

/* --- Mouse events handlers ------------------------------------------------ */

#define MOUSE_SIZE 26

void hideo_handle_mouse_move(hideo_context_t *context, mouse_move_event_t event)
{
    // Restor the damaged region of the screen
    int oldx = context->mouse.x;
    int oldy = context->mouse.y;

    bitmap_copy(context->mouse.oldscreen, 0, 0, 
                context->screen, oldx, oldy, MOUSE_SIZE, MOUSE_SIZE);

    //sk_io_graphic_blit_region(context->screen->buffer, context->mouse.x, context->mouse.y, MOUSE_SIZE, MOUSE_SIZE);

    context->mouse.x = min(context->width, max(0, context->mouse.x + event.offx));
    context->mouse.y = min(context->height, max(0, context->mouse.y + event.offy));

    int x = context->mouse.x;
    int y = context->mouse.y;
    
    // Save the screen
    bitmap_copy(context->screen, x, y, context->mouse.oldscreen, 0, 0, MOUSE_SIZE, MOUSE_SIZE);

    #define P1 x, y
    #define P2 x, y + MOUSE_SIZE - 1
    #define P3 x + MOUSE_SIZE / 2 + MOUSE_SIZE / 8, y + MOUSE_SIZE / 2 + MOUSE_SIZE / 8

    drawing_filltri(context->screen, P1, P2, P3, 0xffffff);

    drawing_line(context->screen, P1, P2, 0x0);
    drawing_line(context->screen, P1, P3, 0x0);
    drawing_line(context->screen, P2, P3, 0x0);

    #undef P1
    #undef P2
    #undef P3

    int xx = min(x, oldx);
    int yy = min(y, oldy);

    int ww = (max(x, oldx)) - xx + MOUSE_SIZE;
    int hh = (max(y, oldy)) - yy + MOUSE_SIZE;

    // FOR DEBBUGING
    // drawing_fillrect(context->screen, xx, yy, ww, hh, 0xffffff);
    // drawing_rect(context->screen, xx, yy, ww, hh, 0x0);

    sk_io_graphic_blit_region(context->screen->buffer, xx, yy, ww, hh);
}

void hideo_handle_mouse_scroll(hideo_context_t *context, mouse_scroll_event_t event)
{
}

void hideo_handle_mouse_pressed(hideo_context_t *context, mouse_button_event_t event)
{
}

void hideo_handle_mouse_released(hideo_context_t *context, mouse_button_event_t event)
{
}

/* --- Keyboards events handlers -------------------------------------------- */

void hideo_handle_keyboard_pressed(hideo_context_t *context, keyboard_event_t event)
{
}

void hideo_handle_keyboard_released(hideo_context_t *context, keyboard_event_t event)
{
}

void hideo_handle_keyboard_typed(hideo_context_t *context, keyboard_event_t event)
{
    if (event.key == Q)
    {
        die("Exited!");
    }
}

/* --- Main functions ------------------------------------------------------- */

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    sk_logger_setlevel(LOG_ALL);

    // Setup the graphic context
    uint width, height = 0;
    sk_io_graphic_size(&width, &height);

    if (width < 800 || height < 600)
    {
        die("Invalid graphic context!");
    }

    // Create the hideo context
    hideo_context_t context = {
        .running = true,
        .width = width,
        .height = height,
        .screen = bitmap(width, height),
        .windows = list(),
        .focus = NULL,

        .mouse = {
            .x = width / 2,
            .y = height / 2,
            .oldscreen = bitmap(MOUSE_SIZE, MOUSE_SIZE),
        }};

    drawing_clear(context.mouse.oldscreen, 0xc1c1c1);
    drawing_clear(context.screen, 0xc1c1c1);
    
    // Enter the message loop
    sk_messaging_subscribe(KEYBOARD_CHANNEL);
    sk_messaging_subscribe(MOUSE_CHANNEL);

    while (context.running)
    {
        message_t msg;
        sk_messaging_receive(&msg);

        //sk_log(LOG_DEBUG, "hideo msg: %s", msg.label);

        if (strcmp(msg.label, MOUSE_MOVE) == 0)
        {
            mouse_move_event_t event;
            sk_messaging_payload(&event, sizeof(mouse_move_event_t));
            hideo_handle_mouse_move(&context, event);
        }
        else if (strcmp(msg.label, MOUSE_SCROLL) == 0)
        {
            mouse_scroll_event_t event;
            sk_messaging_payload(&event, sizeof(mouse_scroll_event_t));
            hideo_handle_mouse_scroll(&context, event);
        }
        else if (strcmp(msg.label, MOUSE_BUTTONDOWN) == 0)
        {
            mouse_button_event_t event;
            sk_messaging_payload(&event, sizeof(mouse_button_event_t));
            hideo_handle_mouse_pressed(&context, event);
        }
        else if (strcmp(msg.label, MOUSE_BUTTONUP) == 0)
        {
            mouse_button_event_t event;
            sk_messaging_payload(&event, sizeof(mouse_button_event_t));
            hideo_handle_mouse_released(&context, event);
        }
        else if (strcmp(msg.label, KEYBOARD_KEYPRESSED) == 0)
        {
            keyboard_event_t event;
            sk_messaging_payload(&event, sizeof(keyboard_event_t));
            hideo_handle_keyboard_pressed(&context, event);
        }
        else if (strcmp(msg.label, KEYBOARD_KEYRELEASED) == 0)
        {
            keyboard_event_t event;
            sk_messaging_payload(&event, sizeof(keyboard_event_t));
            hideo_handle_keyboard_released(&context, event);
        }
        else if (strcmp(msg.label, KEYBOARD_KEYTYPED) == 0)
        {
            keyboard_event_t event;
            sk_messaging_payload(&event, sizeof(keyboard_event_t));
            hideo_handle_keyboard_typed(&context, event);
        }
    }

    return 0;
}
