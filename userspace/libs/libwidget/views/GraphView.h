#pragma once

#include <libwidget/Element.h>
#include <libwidget/model/GraphModel.h>

namespace Widget
{

struct GraphView : public Element
{
private:
    Ref<GraphModel> _model = nullptr;
    Graphic::Color _color;
    Box<Async::Observer<GraphModel>> _observer;

public:
    GraphView(Ref<GraphModel> model, Graphic::Color color);

    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override;
};

WIDGET_BUILDER(GraphView, graph);

} // namespace Widget
