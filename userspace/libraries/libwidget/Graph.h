#pragma once

#include <libwidget/Component.h>

namespace Widget
{

class Graph : public Component
{
private:
    Graphic::Color _color;
    float *_data;
    size_t _data_size;
    size_t _current;

public:
    Graph(Component *parent, size_t data_size, Graphic::Color data_color);

    ~Graph();

    void record(float data);

    float average()
    {
        if (_data_size == 0)
        {
            return 0;
        }

        float total = 0;

        for (size_t i = 0; i < MIN(_current, _data_size); i++)
        {
            total += _data[i];
        }

        return total / MIN(_current, _data_size);
    }

    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override;

    Math::Vec2i size() override;
};

} // namespace Widget
