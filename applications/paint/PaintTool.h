#pragma once

#include <libwidget/Event.h>

struct PaintTool;
struct PaintDocument;

typedef void (*PaintToolMouseEventCallback)(struct PaintTool *tool, struct PaintDocument *document, Event event);

typedef void (*PaintToolDestroyCallback)(struct PaintTool *tool);

struct PaintTool
{
    PaintToolMouseEventCallback on_mouse_event;
    PaintToolDestroyCallback destroy;
};

PaintTool *pencil_tool_create();

PaintTool *brush_tool_create();

PaintTool *eraser_tool_create();

PaintTool *fill_tool_create();

PaintTool *picker_tool_create();
