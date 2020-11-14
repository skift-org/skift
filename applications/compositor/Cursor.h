#pragma once

#include <abi/Mouse.h>
#include <libgraphic/Painter.h>

void cursor_initialize();

void cursor_handle_packet(MousePacket packet);

void cursor_render(Painter &painter);

Rectangle cursor_bound_from_position(Vec2i position);

Rectangle cursor_dirty_bound_from_position(Vec2i position);

Rectangle cursor_bound();

Rectangle cursor_dirty_bound();

Vec2i cursor_position();
