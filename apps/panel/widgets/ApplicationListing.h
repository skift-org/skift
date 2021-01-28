#pragma once

#include <libwidget/VScroll.h>

namespace panel
{

class ApplicationListing : public VScroll
{
private:
    String _filter;

public:
    ApplicationListing(Widget *parent);

    void filter(const String &filter);

    void render();
};

} // namespace panel
