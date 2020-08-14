#include <libgraphic/Painter.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libwidget/widgets/Graph.h>

Graph::Graph(Widget *parent, size_t data_size, Color data_color)
    : Widget(parent)
{
    _data = (double *)calloc(data_size, sizeof(double));
    _data_size = data_size;
    _color = data_color;
    _current = 0;
}

Graph::~Graph()
{
    free(_data);
}

void Graph::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    auto distance = [](double from, double to, int size) {
        if (from > to)
        {
            from -= size;
        }

        return abs(to - from) / (double)size;
    };

    auto graph_sample = [&](float where) {
        assert(where >= 0 && where <= 1);

        return _data[(size_t)(_data_size * where)];
    };

    double cursor_position = _current / (double)_data_size;

    for (int i = 0; i < bound().width(); i++)
    {
        double where = i / (double)bound().width();
        double data = graph_sample(where);

        Rectangle bar(
            bound().x() + i,
            (int)(bound().y() + bound().height() * (1.0 - data)),
            1,
            bound().height());

        double dist = (1 - distance(where, cursor_position, 1)) * 0.5;

        painter.fill_rectangle(bar, ALPHA(_color, dist));
        painter.plot_pixel(bar.position(), _color);
    }

    Rectangle cursor(
        (int)(bound().x() + bound().width() * cursor_position),
        bound().y(),
        1,
        bound().height());

    painter.fill_rectangle(cursor, color(THEME_BORDER));
}

Vec2i Graph::size()
{
    return Vec2i(_data_size, 100);
}

void Graph::record(double data)
{
    _data[_current] = data;
    _current = (_current + 1) % _data_size;

    should_repaint();
}
