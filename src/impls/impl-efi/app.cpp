module Karm.App._embed;
import Karm.App;

namespace Karm::App::_Embed {

static Opt<MockPrefs> _globalPrefs;

Prefs& globalPrefs() {
    if (not _globalPrefs)
        _globalPrefs = MockPrefs{};
    return *_globalPrefs;
}

} // namespace Karm::App::_Ember
