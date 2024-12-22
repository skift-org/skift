

#include <karm-app/_embed.h>
#include <karm-app/prefs.h>

namespace Karm::App::_Ember {

static Opt<MockPrefs> _globalPrefs;

Prefs &globalPrefs() {
    if (not _globalPrefs)
        _globalPrefs = MockPrefs{};
    return *_globalPrefs;
}

} // namespace Karm::App::_Ember
