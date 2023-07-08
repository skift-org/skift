#pragma once

#include <karm-ui/react.h>

namespace ImageViewer {

/* --- Reducer -------------------------------------------------------------- */

struct State {
    bool isEditor = false;
    Res<Media::Image> image;
    Gfx::Filter filter = Gfx::Unfiltered{};

    State(Res<Media::Image> i) : image(i) {}
};

struct Refresh {};

struct ToggleEditor {};

struct SetFilter {
    Gfx::Filter filter;
};

struct ApplyFilter {};

struct SaveImage {};

using Actions = Var<
    Refresh,
    ToggleEditor,
    SetFilter,
    ApplyFilter,
    SaveImage>;

State reduce(State s, Actions action);

using Model = Ui::Model<State, Actions, reduce>;

/* --- Views ---------------------------------------------------------------- */

// Viewer

Ui::Child viewer(State const &state);

Ui::Child viewerPreview(State const &state);

Ui::Child viewerToolbar(State const &state);

Ui::Child viewerControls(State const &state);

// Editor

Ui::Child editor(State const &state);

Ui::Child editorPreview(State const &state);

Ui::Child editorToolbar(State const &state);

Ui::Child editorControls(State const &state);

} // namespace ImageViewer
