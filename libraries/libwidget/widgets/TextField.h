#pragma once

#include <libwidget/Widget.h>
#include <libwidget/model/TextModel.h>
#include <libwidget/widgets/ScrollBar.h>

class TextField : public Widget
{
private:
    RefPtr<TextModel> _model;
    TextCursor _cursor{};

    ScrollBar *_vscrollbar;
    ScrollBar *_hscrollbar;

    int _vscroll_offset = 0;
    int _hscroll_offset = 0;

    void update_scrollbar();

    void scroll_to_cursor();

public:
    TextField(Widget *parent, RefPtr<TextModel> model);

    ~TextField();

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);

    void do_layout();
};
