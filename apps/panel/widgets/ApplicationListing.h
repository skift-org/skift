#pragma once

#include <libwidget/Container.h>

namespace panel
{

class ApplicationListing : public Container
{
private:
    String _filter;

public:
    ApplicationListing(Widget *parent);

    void filter(const String &filter);

    void render();
};

} // namespace panel
