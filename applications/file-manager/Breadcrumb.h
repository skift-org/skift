#pragma once

#include <libsystem/io/Path.h>
#include <libwidget/Widget.h>

struct Breadcrumb : public Widget
{
private:
    Path *_path;

    RefPtr<Icon> _icon_computer;
    RefPtr<Icon> _icon_expand;

public:
    Breadcrumb(Widget *parent, Path *path);

    ~Breadcrumb();

    void navigate(Path *path);

    void paint(Painter &painter, Rectangle rectangle);
};
