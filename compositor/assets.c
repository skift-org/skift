/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/logger.h>
#include "compositor/assets.h"

hideo_assets_t* hideo_assets(void)
{
    hideo_assets_t* this = OBJECT(hideo_assets);

    logger_log(LOG_INFO, "Loading assets...");

    this->mouse_cursors[MOUSE_CURSOR_STATE_DEFAULT] = bitmap_load_from("/res/mouse/default.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/default.png" "...");
    this->mouse_cursors[MOUSE_CURSOR_STATE_DISABLED] = bitmap_load_from("/res/mouse/disabled.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/disabled.png" "...");
    this->mouse_cursors[MOUSE_CURSOR_STATE_BUSY] = bitmap_load_from("/res/mouse/busy.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/busy.png" "...");
    this->mouse_cursors[MOUSE_CURSOR_STATE_TEXT] = bitmap_load_from("/res/mouse/text.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/text.png" "...");
    this->mouse_cursors[MOUSE_CURSOR_STATE_MOVE] = bitmap_load_from("/res/mouse/move.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/move.png" "...");
    this->mouse_cursors[MOUSE_CURSOR_STATE_RESIZEH] = bitmap_load_from("/res/mouse/resizeh.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/resizeh.png" "...");
    this->mouse_cursors[MOUSE_CURSOR_STATE_RESIZEV] = bitmap_load_from("/res/mouse/resizev.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/resizev.png" "...");
    this->mouse_cursors[MOUSE_CURSOR_STATE_RESIZEHV] = bitmap_load_from("/res/mouse/resizehv.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/resizehv.png" "...");
    this->mouse_cursors[MOUSE_CURSOR_STATE_RESIZEVH] = bitmap_load_from("/res/mouse/resizevh.png");
    logger_log(LOG_DEBUG, "Loading assets:" "/res/mouse/resizevh.png" "...");

    this->font_sans = font("sans");

    logger_log(LOG_INFO, "Assets loaded!");

    return this;
}

void hideo_assets_delete(hideo_assets_t* this)
{
    for (int i = 0; i < MOUSE_CURSOR_STATE_COUNT; i++)
    {
        bitmap_delete(this->mouse_cursors[i]);
    }
    
    font_delete(this->font_sans);
}