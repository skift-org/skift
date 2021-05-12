#include <assert.h>

#include <libgraphic/Painter.h>
#include <libwidget/views/GraphView.h>

namespace Widget
{

GraphView::GraphView(RefPtr<GraphModel> model, Graphic::Color color)
    : _model{model},
      _color{color}
{
    _observer = model->observe([this](auto &) { should_repaint(); });
}

void GraphView::paint(Graphic::Painter &painter, const Math::Recti &)
{
    int height = bound().height();
    int width = bound().width();

    auto distance = [](float from, float to, int size) {
        if (from > to)
        {
            from -= size;
        }

        return fabsf(to - from) / (float)size;
    };

    float cursor_position = _model->current();

    for (int i = 0; i < width; i++)
    {
        float where = i / (float)width;
        float data = _model->sample(where);

        Math::Recti bar{i, (int)(height * (1.0 - data)), 1, height};

        float dist = (1 - distance(where, cursor_position, 1));

        painter.fill_rectangle(bar, _color.with_alpha((dist * dist) * 0.50));
        painter.plot(bar.position(), _color);
    }

    Math::Recti cursor{(int)(width * cursor_position), 0, 1, height};

    painter.fill_rectangle(cursor, color(THEME_BORDER));
}

} // namespace Widget
