#pragma once

#include <libgraphic/Painter.h>

#include <libwidget/Widget.h>

class PaginationDots : public Widget
{
private:
    int _count = 3;
    int _index = 0;

public:
    static constexpr int DOTSIZE = 4;
    static constexpr int DOTSPACING = 8;

    int count()
    {
        return _count;
    }

    void count(int count)
    {
        _count = count;
        max_width(size().x());
        should_relayout();
        should_repaint();
    }

    int index()
    {
        return _index;
    }

    void index(int index)
    {
        should_repaint();
        _index = index;
    }

    PaginationDots(Widget *parent, int count)
        : Widget(parent),
          _count(count)
    {
        max_height(DOTSIZE);
        max_width(size().x());
    }

    ~PaginationDots()
    {
    }

    void paint(Painter &painter, Recti) override
    {
        for (int i = 0; i < _count; i++)
        {
            auto dot = content_bound().column(_count, i, DOTSPACING);

            if (_index == i)
            {
                painter.fill_rectangle_rounded(dot, DOTSIZE, color(THEME_FOREGROUND));
            }
            else
            {
                painter.fill_rectangle_rounded(dot, DOTSIZE, color(THEME_BORDER));
            }
        }
    }

    Vec2i size() override
    {
        return {DOTSIZE * _count + DOTSPACING * (_count - 1), DOTSIZE};
    }
};