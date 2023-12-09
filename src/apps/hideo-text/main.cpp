#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>

namespace Hideo::Text {

Ui::Child app() {
    auto toolbar = Ui::toolbar(
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FILE),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FOLDER),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE_PLUS),
        Ui::separator(),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::UNDO),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::REDO));

    return Ui::scafold({
        .icon = Mdi::TEXT,
        .title = "Text",
        .body = toolbar,
        .size = {700, 500},
    });
}

} // namespace Hideo::Text

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Text::app());
}
