#include <libutils/FuzzyMatcher.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

#include "panel/model/MenuEntry.h"
#include "panel/widgets/ApplicationListing.h"

using namespace Widget;

namespace Panel
{

ApplicationListingComponent::ApplicationListingComponent(String filter) : _filter{filter}
{
    flags(Element::FILL);
}

RefPtr<Element> ApplicationListingComponent::build()
{
    FuzzyMatcher matcher;

    Vector<RefPtr<Element>> childs;

    MenuEntry::load().foreach ([&](auto &entry) {
        if (!matcher.match(_filter, entry.name))
        {
            return Iteration::CONTINUE;
        }

        childs.push_back(basic_button(entry.image, entry.name, [this, entry] {
            process_run(entry.command.cstring(), nullptr, 0);
            window()->hide();
        }));

        return Iteration::CONTINUE;
    });

    if (childs.count() == 0)
    {
        childs.push_back(label("No application found!", Anchor::CENTER));
    }

    return Widget::scroll(vflow(4, childs));
}

} // namespace Panel
