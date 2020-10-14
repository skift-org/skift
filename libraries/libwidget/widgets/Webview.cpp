#include <libgraphic/Painter.h>
#include <libsystem/core/CString.h>
#include <libwidget/Window.h>

#include <libwidget/Widget.h>
#include <libwidget/widgets/Webview.h>
Webview::Webview(Widget *parent, const char* data)
    : Widget(parent)
{
    __unused(data);
    
}

Webview::~Webview()
{
}

void Webview::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.fill_rounded_rectangle(bound(),4, Colors::WHITE);

}
void Webview::event(Event *event)
{
    __unused(event);
    // do nothing for the moment
}