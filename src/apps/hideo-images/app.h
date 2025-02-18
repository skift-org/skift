#pragma once

#include "model.h"

namespace Hideo::Images {

// Viewer

Ui::Child viewer(State const& state);

Ui::Child viewerPreview(State const& state);

Ui::Child viewerToolbar(State const& state);

Ui::Child viewerControls(State const& state);

// Editor

Ui::Child editor(State const& state);

Ui::Child editorPreview(State const& state);

Ui::Child editorToolbar(State const& state);

Ui::Child editorControls(State const& state);

} // namespace Hideo::Images
