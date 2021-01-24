#pragma once

#include <libwidget/Widget.h>
#include <libwidget/model/TextModel.h>

class TextField : public Widget
{
private:
    RefPtr<TextModel> _model;
    OwnPtr<Observer<TextModel>> _model_observer;

    TextCursor _cursor{};
    int _hscroll_offset = 0;

public:
    TextField(Widget *parent, RefPtr<TextModel> model);

    ~TextField() override;

    void scroll_to_cursor();

    void paint(Painter &painter, const Recti &rectangle) override;

    Vec2i size() override;

    void event(Event *event) override;
};