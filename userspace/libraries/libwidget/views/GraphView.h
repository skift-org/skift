#pragma once

#include <libwidget/Element.h>
#include <libwidget/model/GraphModel.h>

namespace Widget
{

struct GraphView : public Element
{
private:
    RefPtr<GraphModel> _model = nullptr;
    Graphic::Color _color;
    OwnPtr<Async::Observer<GraphModel>> _observer;

public:
    GraphView(RefPtr<GraphModel> model, Graphic::Color color);

    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override;
};

WIDGET_BUILDER(GraphView, graph);

} // namespace Widget
