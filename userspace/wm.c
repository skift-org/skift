#include <skift/logger.h>
#include <skift/iostream.h>
#include <skift/messaging.h>
#include <skift/drawing.h>
#include <skift/error.h>

#include <skift/widgets.h>

#include <hjert/devices/keyboard.h>
#include <hjert/devices/mouse.h>
#include <hjert/devices/framebuffer.h>

typedef struct
{
    point_t position;
    mouse_cursor_state_t state;
    bool has_mask;
    painter_t *paint;
    bitmap_t *mask;
} wmmouse_t;

typedef struct
{
    bool exited;
    bitmap_t *framebuffer;
    painter_t *paint;

    iostream_t *device;
    wmmouse_t mouse;

    bitmap_t *cursors[MOUSE_CURSOR_STATE_COUNT];
    bitmap_t *wallpaper;
} wmanager_t;

void wmanager_load_assets(wmanager_t *wm)
{
    logger_log(LOG_INFO, "Loading assets...");

    wm->cursors[MOUSE_CURSOR_STATE_DEFAULT] = bitmap_load_from("/res/mouse/default.png");
    wm->cursors[MOUSE_CURSOR_STATE_DISABLED] = bitmap_load_from("/res/mouse/disabled.png");
    wm->cursors[MOUSE_CURSOR_STATE_BUSY] = bitmap_load_from("/res/mouse/busy.png");
    wm->cursors[MOUSE_CURSOR_STATE_TEXT] = bitmap_load_from("/res/mouse/text.png");
    wm->cursors[MOUSE_CURSOR_STATE_MOVE] = bitmap_load_from("/res/mouse/move.png");
    wm->cursors[MOUSE_CURSOR_STATE_RESIZEH] = bitmap_load_from("/res/mouse/resizeh.png");
    wm->cursors[MOUSE_CURSOR_STATE_RESIZEV] = bitmap_load_from("/res/mouse/resizev.png");
    wm->cursors[MOUSE_CURSOR_STATE_RESIZEHV] = bitmap_load_from("/res/mouse/resizehv.png");
    wm->cursors[MOUSE_CURSOR_STATE_RESIZEVH] = bitmap_load_from("/res/mouse/resizevh.png");

    wm->wallpaper = bitmap_load_from("/res/wallpaper/brand.png");

    logger_log(LOG_FINE, "Loading assets");
}

void wmanager_open_display(wmanager_t *wm)
{
    wm->device = iostream_open(FRAMEBUFFER_DEVICE, IOSTREAM_READ);

    if (wm->device == NULL)
    {
        error_print("Failled to open " FRAMEBUFFER_DEVICE);
        wm->exited = true;
    }

    framebuffer_mode_info_t mode_info = {true, 800, 600};

    if (iostream_ioctl(wm->device, FRAMEBUFFER_IOCTL_SET_MODE, &mode_info) < 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        wm->exited = true;
    }

    wm->framebuffer = bitmap(800, 600);
    wm->paint = painter(wm->framebuffer);
}

void wmanager_damage_region(wmanager_t* wm, rectangle_t region)
{
    framebuffer_region_t damaged_region = (framebuffer_region_t){.src = wm->framebuffer->buffer, .bound = region};
    iostream_ioctl(wm->device, FRAMEBUFFER_IOCTL_BLITREGION, &damaged_region);
}

rectangle_t wmanager_mouse_bound(wmmouse_t *mouse)
{
    rectangle_t damage;

    damage.position = point_add(mouse->position, (point_t){-28, -28});
    damage.size = (point_t){56, 56};
    
    return damage;
}

void wmanager_handle_mouse_move_event(wmanager_t *wm, mouse_move_event_t *event)
{
    wmmouse_t *mouse = &wm->mouse;

    // Cleanup the backbuffer
    if (mouse->has_mask)
    {
        painter_blit_bitmap(wm->paint, mouse->mask, bitmap_bound(mouse->mask), wmanager_mouse_bound(mouse));
        wmanager_damage_region(wm, wmanager_mouse_bound(mouse));
    }

    // Move the mouse
    wm->mouse.position = point_add(wm->mouse.position, (point_t){event->offx, event->offy});
    wm->mouse.position = point_clamp(wm->mouse.position, point_zero, bitmap_bound(wm->framebuffer).size);

    // Save the backbuffer
    painter_blit_bitmap(mouse->paint, wm->framebuffer, wmanager_mouse_bound(mouse), bitmap_bound(mouse->mask));
    mouse->has_mask = true;

    // Draw the mouse cursor to the backbuffer
    point_t offset;
    switch (mouse->state)
    {
    case MOUSE_CURSOR_STATE_TEXT:
        offset = (point_t){-14, 0};
        break;

    case MOUSE_CURSOR_STATE_BUSY:
    case MOUSE_CURSOR_STATE_MOVE:
    case MOUSE_CURSOR_STATE_RESIZEH:
    case MOUSE_CURSOR_STATE_RESIZEV:
    case MOUSE_CURSOR_STATE_RESIZEHV:
    case MOUSE_CURSOR_STATE_RESIZEVH:
        offset = (point_t){-14, -14};
        break;

    default:
        offset = (point_t){0, 0};
        break;
    }

    painter_blit_bitmap(
        wm->paint,
        wm->cursors[mouse->state],
        bitmap_bound(wm->cursors[mouse->state]),
        (rectangle_t){.position = point_add(wm->mouse.position, offset), .size = bitmap_bound(wm->cursors[mouse->state]).size});

    wmanager_damage_region(wm, wmanager_mouse_bound(mouse));
}

void wmanager_handle_mouse_scroll_event(wmanager_t *wm, mouse_scroll_event_t *event)
{
    UNUSED(wm);
    UNUSED(event);
}

void wmanager_handle_mouse_btnup_event(wmanager_t *wm, mouse_button_event_t *event)
{
    UNUSED(wm);
    UNUSED(event);
}

void wmanager_handle_mouse_btndown_event(wmanager_t *wm, mouse_button_event_t *event)
{
    UNUSED(wm);
    UNUSED(event);

    wm->mouse.state = (wm->mouse.state + 1) % MOUSE_CURSOR_STATE_COUNT;
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    wmanager_t wm;
    wm.exited = false;

    wmanager_load_assets(&wm);
    wmanager_open_display(&wm);

    wm.mouse.position = (point_t){wm.framebuffer->width / 2, wm.framebuffer->height / 2};
    wm.mouse.state = MOUSE_CURSOR_STATE_DEFAULT;
    wm.mouse.has_mask = false;
    wm.mouse.mask = bitmap(56, 56);
    wm.mouse.paint = painter(wm.mouse.mask);

    messaging_subscribe(MOUSE_CHANNEL);
    messaging_subscribe(KEYBOARD_CHANNEL);

    painter_blit_bitmap(wm.paint, wm.wallpaper, bitmap_bound(wm.wallpaper), bitmap_bound(wm.framebuffer));

    while (!wm.exited)
    {
        message_t message;
        messaging_receive(&message, true);

        if (message_is(message, MOUSE_MOVE))
        {
            wmanager_handle_mouse_move_event(&wm, message_payload_as(message, mouse_move_event_t));
        }
        else if (message_is(message, MOUSE_SCROLL))
        {
            wmanager_handle_mouse_scroll_event(&wm, message_payload_as(message, mouse_scroll_event_t));
        }
        else if (message_is(message, MOUSE_BUTTONUP))
        {
            wmanager_handle_mouse_btnup_event(&wm, message_payload_as(message, mouse_button_event_t));
        }
        else if (message_is(message, MOUSE_BUTTONDOWN))
        {
            wmanager_handle_mouse_btndown_event(&wm, message_payload_as(message, mouse_button_event_t));
        }
        else
        {
            logger_log(LOG_WARNING, "Unknow message %s!", message_label(message));
        }
    }

    return 0;
}