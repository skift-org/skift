module;

#include <karm-logger/logger.h>
#include <karm-math/align.h>
#include <karm-sys/file.h>

export module Hideo.Text;

import Mdi;
import Karm.Ui;
import Karm.Kira;
import Hideo.Files;

using namespace Karm;

namespace Hideo::Text {

struct State {
    Opt<Ref::Url> url;
    Opt<Error> error;
    Rc<Ui::TextModel> text;
};

struct New {
};

struct Save {
    bool prompt = false;
};

using Action = Union<Ui::TextAction, New, Save>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(::Visitor{
        [&](Ui::TextAction& t) {
            s.text->reduce(t);
        },
        [&](New&) {
            s.url = NONE;
            s.error = NONE;
            s.text = makeRc<Ui::TextModel>();
        },
        [&](Save&) {

        },
    });

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

Ui::Child editor(Rc<Ui::TextModel> text) {
    return Ui::input(text, [](Ui::Node& n, Action a) {
               Model::bubble(n, a);
           }) |
           Ui::insets(16) | Ui::vscroll() | Ui::grow();
}

export Ui::Child app(Opt<Ref::Url> url, Res<String> str) {
    auto text = makeRc<Ui::TextModel>();
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
        [](State const& s) {
            return Kr::scaffold({
                .icon = Mdi::TEXT,
                .title = "Text"s,
                .startTools = [&] -> Ui::Children {
                    return {
                        Ui::button(Model::bind<New>(), Ui::ButtonStyle::subtle(), Mdi::FILE),
                        Ui::button(
                            [](auto& n) {
                                Ui::showDialog(
                                    n,
                                    Files::openDialog([](auto&, auto url) {
                                        logInfo("selected file: {}", url);
                                    })
                                );
                            },
                            Ui::ButtonStyle::subtle(), Mdi::FOLDER
                        ),
                        Ui::button(
                            Model::bindIf(s.text->dirty(), Save{}),
                            Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE
                        ),
                        Ui::button(
                            Model::bindIf(s.text->dirty(), Save{true}), Ui::ButtonStyle::subtle(),
                            Mdi::CONTENT_SAVE_PLUS
                        ),
                    };
                },
                .endTools = [&] -> Ui::Children {
                    return {
                        Ui::button(
                            Model::bindIf<Ui::TextAction>(s.text->canUndo(), Ui::TextAction::UNDO),
                            Ui::ButtonStyle::subtle(),
                            Mdi::UNDO
                        ),
                        Ui::button(
                            Model::bindIf<Ui::TextAction>(s.text->canRedo(), Ui::TextAction::REDO),
                            Ui::ButtonStyle::subtle(),
                            Mdi::REDO
                        )
                    };
                },
                .body = [=] {
                    return Ui::vflow(
                        Ui::hflow(
                            0,
                            Math::Align::CENTER,
                            Ui::labelSmall("{}{}", s.url ? s.url->basename() : "Untitled", s.text->dirty() ? "*" : ""),
                            Ui::icon(Mdi::CIRCLE_SMALL, Ui::GRAY700) | Ui::insets({0, -3}),
                            Ui::text(Ui::TextStyles::labelSmall().withColor(Ui::GRAY500), "{}", s.url)
                        ) | Ui::insets({6, 16}),
                        Kr::separator(),

                        s.error
                            ? Kr::errorPage(Mdi::ALERT_DECAGRAM, "Unable to load text"s, Io::toStr(s.error)) | Ui::grow()
                            : editor(s.text),
                        Kr::separator(),
                        Ui::hflow(
                            6,
                            Math::Align::CENTER,
                            Ui::labelSmall("{}", s.text->dirty() ? "Edited" : ""),
                            Ui::grow(NONE),
                            Ui::labelSmall("Ln {}, Col {}", 0, 0),
                            Kr::separator(),
                            Ui::labelSmall("UTF-8"),
                            Kr::separator(),
                            Ui::labelSmall("LF")
                        ) | Ui::box({
                                .padding = {6, 12},
                                .backgroundFill = Ui::GRAY900,
                            })
                    );
                },
            });
        }
    );
}

} // namespace Hideo::Text
