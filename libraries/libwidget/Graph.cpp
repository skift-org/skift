#include <assert.h>
#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libwidget/Graph.h>

Graph::Graph(Widget *parent, size_t data_size, Color data_color)
    : Widget(parent)
{
    _data = (float *)calloc(data_size, sizeof(float));
    _data_size = data_size;
    _color = data_color;
    _current = 0;
}

Graph::~Graph()
{
    free(_data);
}

void Graph::paint(Painter &painter, const Recti &)
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

    auto graph_sample = [&](float where) {
        assert(where >= 0 && where <= 1);

        return _data[(size_t)(_data_size * where)];
    };

    float cursor_position = (_current % _data_size) / (float)_data_size;

    for (int i = 0; i < width; i++)
    {
        float where = i / (float)width;
        float data = graph_sample(where);

        Recti bar{i, (int)(height * (1.0 - data)), 1, height};

        float dist = (1 - distance(where, cursor_position, 1));

        painter.fill_rectangle(bar, _color.with_alpha((dist * dist) * 0.50));
        painter.plot(bar.position(), _color);
    }

    Recti cursor{(int)(width * cursor_position), 0, 1, height};

    painter.fill_rectangle(cursor, color(THEME_BORDER));
}

Vec2i Graph::size()
{
    return Vec2i(_data_size, 100);
}

void Graph::record(float data)
{
    _data[_current % _data_size] = data;
    _current++;

    should_repaint();
}
