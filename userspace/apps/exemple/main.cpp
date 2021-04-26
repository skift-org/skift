#include <libwidget/Application.h>
#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>
#include <libwidget/components/StatefullComponent.h>

using namespace Widget;

struct CounterComponent :
    public StatefullComponent<int>
{
    RefPtr<Element> build()
    {
        return hflow({
            fill(label(IO::format("You clicked {} times", state()))),
            basic_button("Click Me!", [this] { state(state() + 1); }),
        });
    }
};

WIDGET_BUILDER(CounterComponent, counter);

struct MainWindow : public Window
{
    MainWindow() : Window() {}

    RefPtr<Element> build() override
    {
        return vflow({
            titlebar(Graphic::Icon::get("duck"), "exemple"),
            spacing(16, counter()),
        });
    }
};

int main(int, char const *[])
{
    auto win = new MainWindow();
    win->resize_to_content();
    win->show();
    return Application::the()->run();
}
