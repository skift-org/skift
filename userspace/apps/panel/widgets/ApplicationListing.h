#pragma once

#include "libwidget/Element.h"
#include <libwidget/Components.h>

namespace panel
{

class ApplicationListing : public Widget::Component
{
private:
    String _filter;

public:
    ApplicationListing(String filter);

    RefPtr<Widget::Element> build();
};

WIDGET_BUILDER(ApplicationListing, application_listing);

} // namespace panel
