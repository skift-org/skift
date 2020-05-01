#include <libsystem/assert.h>
#include <libsystem/utils/List.h>

#include "paint/PaintDocument.h"
#include "paint/PaintTool.h"

void pencil_tool_mouse_event(PaintTool *tool, PaintDocument *document, MouseEvent event)
{
    __unused(tool);

    if (event.event.type == EVENT_MOUSE_MOVE || event.event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        Point from = event.old_position;
        Point to = event.position;

        if (event.buttons & MOUSE_BUTTON_LEFT)
        {
            painter_draw_line(document->painter, from, to, document->primary_color);
            document->dirty = true;
        }
        else if (event.buttons & MOUSE_BUTTON_RIGHT)
        {
            painter_draw_line(document->painter, from, to, document->secondary_color);
            document->dirty = true;
        }
    }
}

PaintTool *pencil_tool_create(void)
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = pencil_tool_mouse_event;

    return tool;
}

void brush_tool_mouse_event(PaintTool *tool, PaintDocument *document, MouseEvent event)
{
    __unused(tool);

    if (event.event.type == EVENT_MOUSE_MOVE || event.event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        if (event.buttons & MOUSE_BUTTON_LEFT)
        {
            painter_fill_rectangle(document->painter, RECTANGLE(event.position.X - 16, event.position.Y - 16, 32, 32), document->primary_color);
            document->dirty = true;
        }
        else if (event.buttons & MOUSE_BUTTON_RIGHT)
        {
            painter_fill_rectangle(document->painter, RECTANGLE(event.position.X - 16, event.position.Y - 16, 32, 32), document->secondary_color);
            document->dirty = true;
        }
    }
}

PaintTool *brush_tool_create(void)
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = brush_tool_mouse_event;

    return tool;
}

void eraser_tool_mouse_event(PaintTool *tool, PaintDocument *document, MouseEvent event)
{
    __unused(tool);

    if (event.event.type == EVENT_MOUSE_MOVE || event.event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        if (event.buttons & MOUSE_BUTTON_LEFT)
        {
            painter_clear_rectangle(document->painter, RECTANGLE(event.position.X - 16, event.position.Y - 16, 32, 32), COLOR_RGBA(0, 0, 0, 0));
            document->dirty = true;
        }
        else if (event.buttons & MOUSE_BUTTON_RIGHT)
        {
            painter_fill_rectangle(document->painter, RECTANGLE(event.position.X - 16, event.position.Y - 16, 32, 32), document->secondary_color);
            document->dirty = true;
        }
    }
}

PaintTool *eraser_tool_create(void)
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = eraser_tool_mouse_event;

    return tool;
}

static void flood_fill(Bitmap *bitmap, Point position, Color target, Color fill)
{
    if (!rectangle_containe_point(bitmap_bound(bitmap), position))
    {
        return;
    }

    if (color_equals(bitmap_get_pixel(bitmap, position), fill))
    {
        return;
    }

    if (!color_equals(bitmap_get_pixel(bitmap, position), target))
    {
        return;
    }

    bitmap_set_pixel(bitmap, position, fill);

    flood_fill(bitmap, point_add(position, (Point){1, 0}), target, fill);
    flood_fill(bitmap, point_add(position, (Point){-1, 0}), target, fill);
    flood_fill(bitmap, point_add(position, (Point){0, 1}), target, fill);
    flood_fill(bitmap, point_add(position, (Point){0, -1}), target, fill);
}

void fill_tool_mouse_event(PaintTool *tool, PaintDocument *document, MouseEvent event)
{
    __unused(tool);

    if (event.event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        Color fill_color = COLOR_MAGENTA;

        if (event.buttons & MOUSE_BUTTON_LEFT)
        {
            fill_color = document->primary_color;
        }
        else if (event.buttons & MOUSE_BUTTON_RIGHT)
        {
            fill_color = document->secondary_color;
        }
        else
        {
            return;
        }

        Color target_color = bitmap_get_pixel(document->bitmap, event.position);

        flood_fill(document->bitmap, event.position, target_color, fill_color);

        document->dirty = true;
    }
}

PaintTool *fill_tool_create(void)
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = fill_tool_mouse_event;

    return tool;
}

void picker_tool_mouse_event(PaintTool *tool, PaintDocument *document, MouseEvent event)
{
    __unused(tool);

    if (event.event.type == EVENT_MOUSE_BUTTON_PRESS)
    {
        if (event.buttons & MOUSE_BUTTON_LEFT)
        {
            document->primary_color = bitmap_get_pixel(document->bitmap, event.position);
        }
        else if (event.buttons & MOUSE_BUTTON_RIGHT)
        {
            document->secondary_color = bitmap_get_pixel(document->bitmap, event.position);
        }
    }
}

PaintTool *picker_tool_create(void)
{
    PaintTool *tool = __create(PaintTool);

    tool->on_mouse_event = picker_tool_mouse_event;

    return tool;
}
