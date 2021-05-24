#include <libtext/FuzzyMatcher.h>
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
    Text::FuzzyMatcher matcher;

    Vector<RefPtr<Element>> children;

    MenuEntry::load().foreach([&](auto &entry) {
        if (!matcher.match(_filter, entry.name))
        {
            return Iteration::CONTINUE;
        }

        children.push_back(basic_button(entry.image, entry.name, [this, entry] {
            process_run(entry.command.cstring(), nullptr, 0);
            window()->hide();
        }));

        return Iteration::CONTINUE;
    });

    if (children.count() == 0)
    {
        children.push_back(label("No application found!", Math::Anchor::CENTER));
    }

    return Widget::scroll(vflow(4, children));
}

} // namespace Panel
