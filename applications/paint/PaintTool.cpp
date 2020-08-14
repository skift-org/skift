#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libutils/Vector.h>

#include "paint/PaintDocument.h"
#include "paint/PaintTool.h"

void PencilTool::event(PaintDocument &document, Event &event, Color &color)
{
    if (event.type == Event::MOUSE_MOVE || event.type == Event::MOUSE_BUTTON_PRESS)
    {
        Vec2i from = event.mouse.old_position;
        Vec2i to = event.mouse.position;

        if (event.mouse.buttons & (MOUSE_BUTTON_LEFT | MOUSE_BUTTON_RIGHT))
        {
            document.painter().draw_line_antialias(from, to, color);
            document.dirty(true);
        }
    }
}

void BrushTool::event(PaintDocument &document, Event &event, Color &color)
{
    if (event.type == Event::MOUSE_MOVE ||
        event.type == Event::MOUSE_BUTTON_PRESS)
    {
        if (event.mouse.buttons & (MOUSE_BUTTON_LEFT | MOUSE_BUTTON_RIGHT))
        {
            document.painter().fill_rectangle(Rectangle(event.mouse.position - Vec2i(16), Vec2i(32, 32)), color);
            document.dirty(true);
        }
    }
}

void EraserTool::event(PaintDocument &document, Event &event, Color &color)
{
    if (event.type == Event::MOUSE_MOVE || event.type == Event::MOUSE_BUTTON_PRESS)
    {
        if (event.mouse.buttons & MOUSE_BUTTON_LEFT)
        {
            document.painter().clear_rectangle(Rectangle(event.mouse.position - Vec2i(16), Vec2i(32, 32)), COLOR_RGBA(0, 0, 0, 0));
            document.dirty(true);
        }
        else if (event.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            document.painter().clear_rectangle(Rectangle(event.mouse.position - Vec2i(16), Vec2i(32, 32)), color);
            document.dirty(true);
        }
    }
}

static void flood_fill(Bitmap &bitmap, Vec2i position, Color target, Color fill)
{
    if (!bitmap.bound().containe(position))
        return;

    if (color_equals(bitmap.get_pixel(position), fill))
        return;

    if (!color_equals(bitmap.get_pixel(position), target))
        return;

    auto queue = Vector<Vec2i>(256);
    queue.push_back(position);

    while (!queue.empty())
    {
        Vec2i current = queue.pop_back();

        if (!color_equals(bitmap.get_pixel(current), target))
        {
            continue;
        }

        bitmap.set_pixel(current, fill);

        if (current.x() != 0)
        {
            queue.push_back(current + Vec2i(-1, 0));
        }

        if (current.x() != bitmap.width() - 1)
        {
            queue.push_back(current + Vec2i(1, 0));
        }

        if (current.y() != 0)
        {
            queue.push_back(current + Vec2i(0, -1));
        }

        if (current.y() != bitmap.height() - 1)
        {
            queue.push_back(current + Vec2i(0, 1));
        }
    }
}

void FillTool::event(PaintDocument &document, Event &event, Color &color)
{
    if (event.type == Event::MOUSE_BUTTON_PRESS)
    {
        if (event.mouse.buttons & (MOUSE_BUTTON_LEFT | MOUSE_BUTTON_RIGHT))
        {
            Color target_color = document.bitmap().get_pixel(event.mouse.position);
            flood_fill(document.bitmap(), event.mouse.position, target_color, color);
            document.dirty(true);
        }
    }
}

void PickerTool::event(PaintDocument &document, Event &event, Color &color)
{
    __unused(color);

    if (event.type == Event::MOUSE_BUTTON_PRESS)
    {
        if (event.mouse.buttons & MOUSE_BUTTON_LEFT)
        {
            document.primary_color(document.bitmap().get_pixel(event.mouse.position));
        }
        else if (event.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            document.secondary_color(document.bitmap().get_pixel(event.mouse.position));
        }
    }
}
