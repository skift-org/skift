#pragma once

#include <karm-ui/reducer.h>

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

using Action = Var<
    Refresh,
    ToggleEditor,
    SetFilter,
    ApplyFilter,
    SaveImage>;

void reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

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
