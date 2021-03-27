#pragma once

#include <libwidget/VScroll.h>

namespace panel
{

class ApplicationListing : public VScroll
{
private:
    String _filter;

public:
    ApplicationListing(Component *parent);

    void filter(const String &filter);

    void render();
};

} // namespace panel
