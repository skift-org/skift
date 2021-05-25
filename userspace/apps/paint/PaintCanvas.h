#pragma once

#include <libwidget/Element.h>

#include "paint/PaintDocument.h"
#include "paint/PaintTool.h"

struct PaintCanvas : public Widget::Element
{
private:
    RefPtr<PaintDocument> _document;
    OwnPtr<PaintTool> _tool;

public:
    void tool(OwnPtr<PaintTool> tool) { _tool = tool; }

    Math::Recti paint_area()
    {
        return _document->bound().centered_within(bound());
    }

    PaintCanvas(RefPtr<PaintDocument> document);

    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override;

    void event(Widget::Event *event) override;
};
