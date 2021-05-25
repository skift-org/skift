#pragma once

#include <libgraphic/Color.h>
#include <libwidget/Event.h>

struct PaintDocument;

struct PaintTool
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

struct PencilTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

struct BrushTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

struct EraserTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

struct FillTool : public PaintTool
{
private:
public:
    void event(PaintDocument &document, Widget::Event &event, Graphic::Color &color);
};

struct PickerTool : public PaintTool
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
