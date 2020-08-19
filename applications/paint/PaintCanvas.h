#pragma once

#include <libwidget/Widget.h>

#include "paint/PaintDocument.h"
#include "paint/PaintTool.h"

class PaintCanvas : public Widget
{
private:
    RefPtr<PaintDocument> _document;
    OwnPtr<PaintTool> _tool;

public:
    void tool(OwnPtr<PaintTool> tool) { _tool = tool; }

    Rectangle paint_area()
    {
        return _document->bound().centered_within(bound());
    }

    PaintCanvas(Widget *parent, RefPtr<PaintDocument> document);

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);
};
