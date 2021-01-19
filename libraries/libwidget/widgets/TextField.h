#pragma once

#include <libwidget/Widget.h>
#include <libwidget/model/TextModel.h>

class TextField : public Widget
{
private:
    RefPtr<TextModel> _model;
    TextCursor _cursor{};
    int _hscroll_offset = 0;

public:
    TextField(Widget *parent, RefPtr<TextModel> model);

    ~TextField() override;

    void paint(Painter &painter, Recti rectangle) override;

    void scroll_to_cursor();

    Vec2i size() override;

    void event(Event *event) override;
};