#pragma once

#include <abi/Mouse.h>
#include <libgraphic/Painter.h>

void cursor_initialize();

void cursor_handle_packet(MousePacket packet);

void cursor_render(Graphic::Painter &painter);

Math::Recti cursor_bound_from_position(Math::Vec2i position);

Math::Recti cursor_dirty_bound_from_position(Math::Vec2i position);

Math::Recti cursor_bound();

Math::Recti cursor_dirty_bound();

Math::Vec2i cursor_position();
