#pragma once

#include <libsystem/runtime.h>
#include <libdevice/keyboard.h>
#include <libdevice/mouse.h>

/* --- Mouse events --------------------------------------------------------- */

typedef struct
{
    point_t offset;
    point_t position;
    mouse_button_t button;
} mouse_eventarg_t;

typedef void event_on_mouse_up(object_t *sender, mouse_eventarg_t *e);

typedef void event_on_mouse_down(object_t *sender, mouse_eventarg_t *e);

typedef void event_on_mouse_move(object_t *sender, mouse_eventarg_t *e);

typedef void event_on_mouse_click(object_t *sender, mouse_eventarg_t *e);

typedef void event_on_mouse_double_click(object_t *sender, mouse_eventarg_t *e);

typedef void event_on_mouse_enter(object_t *sender, mouse_eventarg_t *e);

typedef void event_on_mouse_left(object_t *sender, mouse_eventarg_t *e);

/* --- Keyboard events ------------------------------------------------------ */

typedef struct
{
    int codepoint;

} key_eventarg_t;

typedef void event_on_key_down(object_t *sender, key_eventarg_t *e);

typedef void event_on_key_up(object_t *sender, key_eventarg_t *e);

typedef void event_on_key_typed(object_t *sender, key_eventarg_t *e);
