#pragma once

#include <libwidget/Component.h>

#include "paint/PaintDocument.h"
#include "paint/PaintTool.h"

class PaintCanvas : public Widget::Component
{
private:
    RefPtr<PaintDocument> _document;
    OwnPtr<PaintTool> _tool;

public:
    void tool(OwnPtr<PaintTool> tool) { _tool = tool; }

    Recti paint_area()
    {
        return _document->bound().centered_within(bound());
    }

    PaintCanvas(Widget::Component *parent, RefPtr<PaintDocument> document);

    void paint(Graphic::Painter &painter, const Recti &dirty) override;

    void event(Widget::Event *event) override;
};
