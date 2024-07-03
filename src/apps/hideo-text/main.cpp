#include <hideo-base/alert.h>
#include <hideo-base/scafold.h>
#include <hideo-files/widgets.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-ui/app.h>
#include <karm-ui/scroll.h>

namespace Hideo::Text {

struct State {
    Opt<Mime::Url> url;
    Opt<Error> error;
    Strong<Textbox::Model> text;
};

struct New {
};

struct Save {
    bool prompt = false;
};

using Action = Union<Textbox::Action, New, Save>;

void reduce(State &s, Action a) {
    a.visit(::Visitor{
        [&](Textbox::Action &t) {
            s.text->reduce(t);
        },
        [&](New &) {
            s.url = NONE;
            s.error = NONE;
            s.text = makeStrong<Textbox::Model>();
        },
        [&](Save &) {

        },
    });
}

using Model = Ui::Model<State, Action, reduce>;

Ui::Child editor(Strong<Textbox::Model> text) {
    return Ui::input(text, [](Ui::Node &n, Action a) {
               Model::bubble(n, a);
           }) |
           Ui::spacing(16) | Ui::vscroll() | Ui::grow();
}

Ui::Child app(Opt<Mime::Url> url, Res<String> str) {
    auto text = makeStrong<Textbox::Model>();
    Opt<Error> error = NONE;

    if (str) {
        text->load(str.unwrap());
    } else {
        error = str.none();
    }

    return Ui::reducer<Model>(
        State{
            url,
            error,
            text,
        },
        [](State const &s) {
            return scafold({
                .icon = Mdi::TEXT,
                .title = "Text"s,
                .startTools = slots$(
                    Ui::button(Model::bind<New>(), Ui::ButtonStyle::subtle(), Mdi::FILE),
                    Ui::button([](auto &n) {
                        Ui::showDialog(n, Files::openFileDialog());
                    },
                               Ui::ButtonStyle::subtle(), Mdi::FOLDER),
                    Ui::button(Model::bindIf(s.text->dirty(), Save{}), Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE), Ui::button(Model::bindIf(s.text->dirty(), Save{true}), Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE_PLUS)
                ),
                .endTools = slots$(Ui::button(Model::bindIf<Textbox::Action>(s.text->canUndo(), Textbox::Action::UNDO), Ui::ButtonStyle::subtle(), Mdi::UNDO), Ui::button(Model::bindIf<Textbox::Action>(s.text->canRedo(), Textbox::Action::REDO), Ui::ButtonStyle::subtle(), Mdi::REDO)),
                .body = [=] {
                    return Ui::vflow(
                        Ui::hflow(
                            0,
                            Math::Align::CENTER,
                            Ui::labelSmall("{}{}", s.url ? s.url->basename() : "Untitled", s.text->dirty() ? "*" : ""),
                            Ui::icon(Mdi::CIRCLE_SMALL, Ui::GRAY700) | Ui::spacing({-3, 0}),
                            Ui::text(Ui::TextStyles::labelSmall().withColor(Ui::GRAY500), "{}", s.url)
                        ) | Ui::box({
                                .padding = {16, 6},
                            }),
                        Ui::separator(),

                        s.error ? alert("Unable to load text"s, Io::toStr(s.error).unwrap()) | Ui::grow() : editor(s.text),
                        Ui::separator(),
                        Ui::hflow(
                            6,
                            Math::Align::CENTER,
                            Ui::labelSmall("{}", s.text->dirty() ? "Edited" : ""),
                            Ui::grow(NONE),
                            Ui::labelSmall("Ln {}, Col {}", 0, 0),
                            Ui::separator(),
                            Ui::labelSmall("UTF-8"),
                            Ui::separator(),
                            Ui::labelSmall("LF")
                        ) | Ui::box({
                                .padding = {12, 6},
                                .backgroundPaint = Ui::GRAY900,
                            })
                    );
                },
            });
        }
    );
}

Res<String> readAllUtf8(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    return Io::readAllUtf8(file);
}

} // namespace Hideo::Text

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto &args = useArgs(ctx);
    Opt<Mime::Url> url;
    Res<String> text = Ok<String>();
    if (args.len()) {
        url = co_try$(Mime::parseUrlOrPath(args[0]));
        text = Hideo::Text::readAllUtf8(*url);
    }
    co_return Ui::runApp(ctx, Hideo::Text::app(url, text));
}
