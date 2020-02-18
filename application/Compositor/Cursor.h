#pragma once

#include <libdevice/Mouse.h>
#include <libgraphic/Painter.h>

void cursor_initialize(void);

void cursor_handle_packet(MousePacket packet);

void cursor_render(Painter *painter);

Rectangle cursor_bound(void);