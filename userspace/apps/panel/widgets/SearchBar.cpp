#include <libwidget/Elements.h>
#include <libwidget/Views.h>

#include "libwidget/elements/PanelElement.h"
#include "libwidget/layouts/Flags.h"
#include "libwidget/layouts/FlowLayout.h"
#include "libwidget/model/TextModel.h"
#include "libwidget/views/TextField.h"
#include "panel/widgets/SearchBar.h"

namespace panel
{

SearchBar::SearchBar(String text, Callback<void(String)> on_change)
    : _model{Widget::TextModel::create(text)},
      _on_change{on_change}
{
}

RefPtr<Widget::Element> SearchBar::build()
{
    // clang-format off

    return Widget::spacing(4, 
        Widget::panel(6,
            Widget::spacing(8,
                Widget::hflow(4,
                {
                    Widget::icon("search"),
                    Widget::fill(Widget::textfield(_model)),
                })
            )
        )
    );

    // clang-format on
}

} // namespace panel