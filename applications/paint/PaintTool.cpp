#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/utils/Vector.h>

#include "paint/PaintDocument.h"
#include "paint/PaintTool.h"

void pencil_tool_mouse_event(PaintTool *tool, PaintDocument *document, Event event)
{
    __unused(tool);

    if (event.type == EVENT_MOUSE_MOVE || event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        Vec2i from = event.mouse.old_position;
        Vec2i to = event.mouse.position;

        if (event.mouse.buttons & MOUSE_BUTTON_LEFT)
        {
            document->painter.draw_line_antialias(from, to, document->primary_color);
            document->dirty = true;
        }
        else if (event.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            document->painter.draw_line_antialias(from, to, document->secondary_color);
            document->dirty = true;
        }
    }
}

PaintTool *pencil_tool_create()
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = pencil_tool_mouse_event;

    return tool;
}

void brush_tool_mouse_event(PaintTool *tool, PaintDocument *document, Event event)
{
    __unused(tool);

    if (event.type == EVENT_MOUSE_MOVE || event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        Color color = document->primary_color;

        if (event.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            color = document->secondary_color;
        }

        document->painter.fill_rectangle(Rectangle(event.mouse.position, Vec2i(32, 32)), color);

        document->dirty = true;
    }
}

PaintTool *brush_tool_create()
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = brush_tool_mouse_event;

    return tool;
}

void eraser_tool_mouse_event(PaintTool *tool, PaintDocument *document, Event event)
{
    __unused(tool);

    if (event.type == EVENT_MOUSE_MOVE || event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        if (event.mouse.buttons & MOUSE_BUTTON_LEFT)
        {
            document->painter.clear_rectangle(
                Rectangle(
                    event.mouse.position.x() - 16,
                    event.mouse.position.y() - 16,
                    32,
                    32),
                COLOR_RGBA(0, 0, 0, 0));

            document->dirty = true;
        }
        else if (event.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            document->painter.clear_rectangle(
                Rectangle(
                    event.mouse.position.x() - 16,
                    event.mouse.position.y() - 16,
                    32,
                    32),
                document->secondary_color);

            document->dirty = true;
        }
    }
}

PaintTool *eraser_tool_create()
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = eraser_tool_mouse_event;

    return tool;
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

void fill_tool_mouse_event(PaintTool *tool, PaintDocument *document, Event event)
{
    __unused(tool);

    if (event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        Color fill_color = COLOR_MAGENTA;

        if (event.mouse.buttons & MOUSE_BUTTON_LEFT)
        {
            fill_color = document->primary_color;
        }
        else if (event.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            fill_color = document->secondary_color;
        }
        else
        {
            return;
        }

        Color target_color = document->bitmap->get_pixel(event.mouse.position);

        flood_fill(*document->bitmap, event.mouse.position, target_color, fill_color);

        document->dirty = true;
    }
}

PaintTool *fill_tool_create()
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = fill_tool_mouse_event;

    return tool;
}

void picker_tool_mouse_event(PaintTool *tool, PaintDocument *document, Event event)
{
    __unused(tool);

    if (event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        if (event.mouse.buttons & MOUSE_BUTTON_LEFT)
        {
            document->primary_color = document->bitmap->get_pixel(event.mouse.position);
        }
        else if (event.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            document->secondary_color = document->bitmap->get_pixel(event.mouse.position);
        }
    }
}

PaintTool *picker_tool_create()
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = picker_tool_mouse_event;

    return tool;
}
