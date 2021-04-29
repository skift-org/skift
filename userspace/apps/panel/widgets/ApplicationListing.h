#pragma once

#include <libwidget/Elements.h>

namespace panel
{

class ApplicationListing : public Widget::ScrollElement
{
private:
    String _filter;

public:
    ApplicationListing();

    void filter(const String &filter);

    void render();
};

} // namespace panel
