#pragma once

#include <libwidget/Element.h>
#include <libwidget/model/TextModel.h>

namespace Widget
{

class TextField : public Element
{
private:
    RefPtr<TextModel> _model;
    OwnPtr<Async::Observer<TextModel>> _model_observer;

    TextCursor _cursor{};
    int _hscroll_offset = 0;

public:
    TextField(RefPtr<TextModel> model);

    ~TextField() override;

    void scroll_to_cursor();

    void paint(Graphic::Painter &painter, const Math::Recti &rectangle) override;

    Math::Vec2i size() override;

    void event(Event *event) override;

    String text();
};

static inline RefPtr<TextField> textfield(RefPtr<TextModel> model) { return make<TextField>(model); }

} // namespace Widget
