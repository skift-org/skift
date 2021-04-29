#include <libwidget/Elements.h>
#include <libwidget/Views.h>

#include "panel/widgets/SearchBar.h"

namespace panel
{

SearchBar::SearchBar(RefPtr<Widget::TextModel> model) : PanelElement(6)
{
    DONT_USE_ME_layout(HFLOW(4));
    insets({6});
    outsets({8});
    color(Widget::THEME_MIDDLEGROUND, Graphic::Colors::WHITE);
    DONT_USE_ME_layout(HFLOW(4));
    min_height(36);

    auto icon = add(Widget::icon("search"));
    icon->color(Widget::THEME_FOREGROUND, Graphic::Colors::BLACK);

    auto field = add(Widget::textfield(model));
    field->flags(Element::FILL);
    field->color(Widget::THEME_FOREGROUND, Graphic::Colors::BLACK);
    field->focus();
}

} // namespace panel