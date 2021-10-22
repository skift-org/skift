#pragma once

#include <libwidget/Element.h>
#include <libwidget/model/TextModel.h>

namespace Widget
{

struct TextField : public Element
{
private:
    Ref<TextModel> _model;
    Box<Async::Observer<TextModel>> _model_observer;

    TextCursor _cursor{};
    int _hscroll_offset = 0;

public:
    TextField(Ref<TextModel> model);

    ~TextField() override;

    void scroll_to_cursor();

    void paint(Graphic::Painter &painter, const Math::Recti &rectangle) override;

    Math::Vec2i size() override;

    void event(Event *event) override;

    String text();
};

WIDGET_BUILDER(TextField, textfield);

} // namespace Widget
