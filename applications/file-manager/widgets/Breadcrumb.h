#pragma once

#include <libwidget/Widget.h>

#include "file-manager/model/Navigation.h"

namespace file_manager
{

struct Breadcrumb : public Widget
{
private:
    RefPtr<Navigation> _navigation;
    OwnPtr<Observer<Navigation>> _observer;

    RefPtr<Icon> _icon_computer;
    RefPtr<Icon> _icon_expand;

public:
    Breadcrumb(Widget *parent, RefPtr<Navigation> navigation);

    void paint(Painter &painter, Rectangle rectangle);
};

} // namespace file_manager
