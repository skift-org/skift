#include <libwidget/Application.h>

#include "text-editor/TextEditorWindow.h"

int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1] : "";
    auto window = new TextEditorWindow(path);
    window->show();

    return Application::the().run();
}
