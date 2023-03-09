#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>

Res<> entryPoint(CliArgs args) {
    auto content = Ui::vflow(
        16,
        Ui::icon(Mdi::Icon::EMOTICON_HAPPY_OUTLINE, 64) | Ui::center(),
        Ui::titleLarge("Hello, world!") | Ui::center());

    return Ui::runApp(args, content | Ui::spacing(16));
}
