#pragma once

#include <skift/shape.h>

typedef enum
{
    MOUSE_CURSOR_STATE_DEFAULT,
    MOUSE_CURSOR_STATE_DISABLED,
    MOUSE_CURSOR_STATE_BUSY,
    MOUSE_CURSOR_STATE_TEXT,
    MOUSE_CURSOR_STATE_MOVE,
    MOUSE_CURSOR_STATE_RESIZEH,
    MOUSE_CURSOR_STATE_RESIZEV,
    MOUSE_CURSOR_STATE_RESIZEHV,
    MOUSE_CURSOR_STATE_RESIZEVH,
    MOUSE_CURSOR_STATE_COUNT,
} mouse_cursor_state_t;

typedef enum {
    WFLOW_ROW,
    WFLOW_ROW_REVERSE,
    WFLOW_COLUMN,
    WFLOW_COLUMN_REVERSE
} widget_flow_t;

typedef enum
{
    WWRAP_NOWRAP,
    WWRAP_WRAP,
    WWRAP_WRAP_REVERSE
} widget_wrap_t;

typedef enum
{
    WALIGN_START,
    WALIGN_END,
    WALIGN_CENTER,
    WALIGN_SPACE_AROUND,
    WALIGN_SPACE_BETWEEN,
    WALIGN_stretch,
} widget_align_t;

typedef struct
{
    rectangle_t bound;
} widget_t;
