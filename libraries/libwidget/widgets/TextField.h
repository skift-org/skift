#pragma once

#include <libwidget/Widget.h>
#include <libwidget/model/TextModel.h>
#include <libwidget/widgets/ScrollBar.h>

class TextField : public Widget
{
private:
    ScrollBar *_scrollbar;
    RefPtr<TextModel> _model;
    TextCursor _cursor{};
    int _scroll_offset = 0;

public:
    TextField(Widget *parent, RefPtr<TextModel> model);

    ~TextField();

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);

    void do_layout();
};
