#include <libwidget/Application.h>
#include <libwidget/widgets/Webview.h>
int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = new Window(WINDOW_RESIZABLE);
    window->title("Webview test");
    

    window->size(Vec2i(700, 500));

    window->root()->layout(VFLOW(0));


    Webview *webview_widget = nullptr;
    webview_widget = new Webview(window->root(), "<>");

    webview_widget->attributes(LAYOUT_FILL);
    window->show();

    return application_run();
}
