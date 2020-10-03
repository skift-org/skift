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
            int dx, dy, p, x, y = 0;
            int x0 = event.mouse.old_position.x();
            int y0 = event.mouse.old_position.y();
            int x1 = event.mouse.position.x();
            int y1 = event.mouse.position.y();

            dx = x1 - x0;
            dy = y1 - y0;

            x = x0;
            y = y0;

            p = 2 * dy - dx;

            while (x < x1)
            {
                if (p >= 0)
                {
                    document.painter().fill_rectangle(Rectangle(Vec2(x, y) - Vec2i(16), Vec2i(32, 32)), color);
                    y = y + 1;
                    p = p + 2 * dy - 2 * dx;
                }
                else
                {
                    document.painter().fill_rectangle(Rectangle(Vec2(x, y) - Vec2i(16), Vec2i(32, 32)), color);
                    p = p + 2 * dy;
                }
                x = x + 1;
            }

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
            int dx = (event.mouse.position - event.mouse.old_position).x() > 0 ? 1 : -1;
            int dy = (event.mouse.position - event.mouse.old_position).y() > 0 ? 1 : -1;
            Vec2 _old_position = event.mouse.old_position;
            while (1)
            {
                Vec2 diff = event.mouse.position - _old_position;
                document.painter().clear_rectangle(Rectangle(event.mouse.position - Vec2i(16), Vec2i(32, 32)), Colors::BLACKTRANSPARENT);
                event.mouse.old_position = event.mouse.position;
                event.mouse.position += Vec2(dx, dy);
                if (abs(diff.x()) < 32 and abs(diff.y()) < 32)
                {
                    break;
                }
            }
            document.dirty(true);
        }
        else if (event.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            int dx = (event.mouse.position - event.mouse.old_position).x() > 0 ? 1 : -1;
            int dy = (event.mouse.position - event.mouse.old_position).y() > 0 ? 1 : -1;
            Vec2 _old_position = event.mouse.old_position;
            while (1)                                                                                        
            {
                Vec2 diff = event.mouse.position - _old_position;
                document.painter().clear_rectangle(Rectangle(event.mouse.position - Vec2i(16), Vec2i(32, 32)), color);
                event.mouse.old_position = event.mouse.position;
                event.mouse.position += Vec2(dx, dy);
                if (abs(diff.x()) < 32 and abs(diff.y()) < 32)
                {
                    break;
                }
            }
            document.dirty(true);
        }
    }
}

static void flood_fill(Bitmap &bitmap, Vec2i position, Color target, Color fill)
{
    if (!bitmap.bound().contains(position))
        return;

    if (bitmap.get_pixel(position) == fill)
        return;

    if (bitmap.get_pixel(position) != target)
        return;

    auto queue = Vector<Vec2i>(256);
    queue.push_back(position);

    while (!queue.empty())
    {
        Vec2i current = queue.pop_back();

        if (bitmap.get_pixel(current) != target)
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
