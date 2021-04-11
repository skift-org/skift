#pragma once

#include <libwidget/VScroll.h>

namespace panel
{

class ApplicationListing : public Widget::VScroll
{
private:
    String _filter;

public:
    ApplicationListing(Widget::Component *parent);

    void filter(const String &filter);

    void render();
};

} // namespace panel
