module Karm.App;

import Karm.Core;
import :_embed;
import :prefs;

namespace Karm::App::_Embed {

static Opt<MockPrefs> _globalPrefs;

Prefs& globalPrefs() {
    if (not _globalPrefs)
        _globalPrefs = MockPrefs{};
    return *_globalPrefs;
}

} // namespace Karm::App::_Embed
