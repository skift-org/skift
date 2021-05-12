#pragma once

#include <libwidget/Components.h>

namespace Panel
{

struct ApplicationListingComponent :
    public Widget::Component
{
private:
    String _filter;

public:
    ApplicationListingComponent(String filter);

    RefPtr<Widget::Element> build();
};

WIDGET_BUILDER(ApplicationListingComponent, application_listing);

} // namespace Panel
