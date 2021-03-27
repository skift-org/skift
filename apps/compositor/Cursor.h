#pragma once

#include <abi/Mouse.h>
#include <libgraphic/Painter.h>

void cursor_initialize();

void cursor_handle_packet(MousePacket packet);

void cursor_render(Graphic::Painter &painter);

Recti cursor_bound_from_position(Vec2i position);

Recti cursor_dirty_bound_from_position(Vec2i position);

Recti cursor_bound();

Recti cursor_dirty_bound();

Vec2i cursor_position();
