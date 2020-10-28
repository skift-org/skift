#pragma once

#include <libwidget/Widget.h>
#include <libwidget/model/TextModel.h>
#include <libwidget/widgets/ScrollBar.h>

class TextField : public Widget
{
private:
    static constexpr int LINE_HEIGHT = 18;
    static constexpr int LINENUMBERS_WIDTH = 32;

    RefPtr<TextModel> _model;
    TextCursor _cursor{};

    ScrollBar *_vscrollbar;
    ScrollBar *_hscrollbar;

    int _vscroll_offset = 0;
    int _hscroll_offset = 0;

    bool _linenumbers = false;
    bool _multiline = true;
    bool _readonly = false;
    bool _overscroll = false;

    void update_scrollbar();

    void scroll_to_cursor();

    Rectangle linenumbers_bound() const
    {
        if (_linenumbers)
        {
            return bound().take_left(LINENUMBERS_WIDTH);
        }
        else
        {
            return Rectangle::empty();
        }
    }

    Rectangle view_bound() const
    {
        if (_linenumbers)
        {
            return bound().cutoff_left(LINENUMBERS_WIDTH);
        }
        else
        {
            return bound();
        }
    }

    Rectangle document_bound() const
    {
        return Rectangle{
            -_hscroll_offset,
            -_vscroll_offset,
            1024,
            (int)_model->line_count() * LINE_HEIGHT,
        }
            .offset(bound().position());
    }

    Rectangle vscrollbar_bound() const
    {
        return view_bound().take_right(ScrollBar::SIZE).cutoff_botton(ScrollBar::SIZE);
    }

    Rectangle hscrollbar_bound() const
    {
        return view_bound().take_bottom(ScrollBar::SIZE).cutoff_right(ScrollBar::SIZE);
    }

public:
    void linenumbers(bool value)
    {
        _linenumbers = value;
        should_relayout();
        should_repaint();
    }

    void multiline(bool value)
    {
        _multiline = value;
        should_relayout();
        should_repaint();
    }

    void readonly(bool value)
    {
        _readonly = value;
        should_relayout();
        should_repaint();
    }

    void overscroll(bool value)
    {
        _overscroll = value;
        should_relayout();
        should_repaint();
    }

    TextField(Widget *parent, RefPtr<TextModel> model);

    ~TextField();

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);

    void do_layout();
};
