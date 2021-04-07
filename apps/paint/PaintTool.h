#pragma once

#include <libgraphic/Color.h>
#include <libwidget/Event.h>

struct PaintDocument;

class PaintTool
{
private:
public:
    virtual ~PaintTool() {}

    virtual void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color)
    {
        UNUSED(document);
        UNUSED(event);
        UNUSED(color);
    }
};

class PencilTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

class BrushTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

class EraserTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

class FillTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

class PickerTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

PaintTool *pencil_tool_create();

PaintTool *brush_tool_create();

PaintTool *eraser_tool_create();

PaintTool *fill_tool_create();

PaintTool *picker_tool_create();
