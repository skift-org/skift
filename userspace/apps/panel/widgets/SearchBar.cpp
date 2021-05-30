#include <libwidget/Layouts.h>
#include <libwidget/Views.h>

#include "panel/widgets/SearchBar.h"

using namespace Widget;

namespace Panel
{

SearchBarComponent::SearchBarComponent(String text, Func<void(String)> on_change)
    : _model{TextModel::create(text)},
      _on_change{on_change}
{
}

RefPtr<Element> SearchBarComponent::build()
{
    // clang-format off

    return spacing(4,
        panel(6,
            spacing(8,
                hflow(4,
                {
                    icon(Graphic::Icon::get("search")),
                    fill(textfield(_model)),
                })
            )
        )
    );

    // clang-format on
}

} // namespace Panel
