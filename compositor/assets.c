#include "skift/logger.h"
#include "compositor/assets.h"

static bitmap_t *cursors[MOUSE_CURSOR_STATE_COUNT];

static font_t* font_sans;

void hideo_assets_load(void)
{
    logger_log(LOG_INFO, "Loading assets...");

    cursors[MOUSE_CURSOR_STATE_DEFAULT] = bitmap_load_from("/res/mouse/default.png");
    cursors[MOUSE_CURSOR_STATE_DISABLED] = bitmap_load_from("/res/mouse/disabled.png");
    cursors[MOUSE_CURSOR_STATE_BUSY] = bitmap_load_from("/res/mouse/busy.png");
    cursors[MOUSE_CURSOR_STATE_TEXT] = bitmap_load_from("/res/mouse/text.png");
    cursors[MOUSE_CURSOR_STATE_MOVE] = bitmap_load_from("/res/mouse/move.png");
    cursors[MOUSE_CURSOR_STATE_RESIZEH] = bitmap_load_from("/res/mouse/resizeh.png");
    cursors[MOUSE_CURSOR_STATE_RESIZEV] = bitmap_load_from("/res/mouse/resizev.png");
    cursors[MOUSE_CURSOR_STATE_RESIZEHV] = bitmap_load_from("/res/mouse/resizehv.png");
    cursors[MOUSE_CURSOR_STATE_RESIZEVH] = bitmap_load_from("/res/mouse/resizevh.png");

    font_sans = font("sans");

    logger_log(LOG_INFO, "Loading assets loaded!");
}

bitmap_t* hideo_assets_get_mouse(mouse_cursor_state_t state)
{
    return cursors[state];
}

font_t* hideo_assets_get_font(void)
{
    return font_sans;
}