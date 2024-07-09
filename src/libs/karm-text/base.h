#pragma once

#include <karm-base/checked.h>
#include <karm-base/distinct.h>
#include <karm-base/string.h>

namespace Karm::Text {

struct Glyph : public Distinct<usize, struct _GlyphTag> {
    using Distinct<usize, struct _GlyphTag>::Distinct;

    static Glyph const TOFU;
};

constexpr Glyph Glyph::TOFU{Limits<usize>::MIN};

// MARK: FaceId ----------------------------------------------------------------

struct FaceId : public Distinct<usize, struct _FaceIdTag> {
    using Distinct::Distinct;
};

// MARK: FontStyle -------------------------------------------------------------

enum struct FontStyle {
    NORMAL,
    OBLIQUE,
    ITALIC,
};

// MARK: Fonteight -------------------------------------------------------------

struct FontWeight : public Distinct<u16, struct _FontWeightTag> {
    using Distinct::Distinct;

    static FontWeight const THIN;
    static FontWeight const EXTRA_LIGHT;
    static FontWeight const LIGHT;
    static FontWeight const REGULAR;
    static FontWeight const MEDIUM;
    static FontWeight const SEMI_BOLD;
    static FontWeight const BOLD;
    static FontWeight const EXTRA_BOLD;
    static FontWeight const BLACK;
};

constexpr FontWeight FontWeight::THIN{100};
constexpr FontWeight FontWeight::EXTRA_LIGHT{200};
constexpr FontWeight FontWeight::LIGHT{300};
constexpr FontWeight FontWeight::REGULAR{400};
constexpr FontWeight FontWeight::MEDIUM{500};
constexpr FontWeight FontWeight::SEMI_BOLD{600};
constexpr FontWeight FontWeight::BOLD{700};
constexpr FontWeight FontWeight::EXTRA_BOLD{800};
constexpr FontWeight FontWeight::BLACK{900};

// MARK: FontStretch -----------------------------------------------------------

struct FontStretch : public Distinct<u16, struct _FontStretchTag> {
    using Distinct::Distinct;

    static FontStretch const ULTRA_CONDENSED;
    static FontStretch const EXTRA_CONDENSED;
    static FontStretch const CONDENSED;
    static FontStretch const SEMI_CONDENSED;
    static FontStretch const NORMAL;
    static FontStretch const SEMI_EXPANDED;
    static FontStretch const EXPANDED;
    static FontStretch const EXTRA_EXPANDED;
    static FontStretch const ULTRA_EXPANDED;
};

constexpr FontStretch FontStretch::ULTRA_CONDENSED{100};
constexpr FontStretch FontStretch::EXTRA_CONDENSED{200};
constexpr FontStretch FontStretch::CONDENSED{300};
constexpr FontStretch FontStretch::SEMI_CONDENSED{400};
constexpr FontStretch FontStretch::NORMAL{500};
constexpr FontStretch FontStretch::SEMI_EXPANDED{600};
constexpr FontStretch FontStretch::EXPANDED{700};
constexpr FontStretch FontStretch::EXTRA_EXPANDED{800};
constexpr FontStretch FontStretch::ULTRA_EXPANDED{900};

// MARK: Monospace -------------------------------------------------------------

enum struct Monospace {
    NO,
    YES,
};

// MARK: Family ----------------------------------------------------------------

enum struct GenericFamily {
    SERIF,
    SANS_SERIF,
    MONOSPACE,
    CURSIVE,
    FANTASY,

    SYSTEM,
    EMOJI,
    MATH,
    FANGSONG,

    _LEN,
};

using Family = Union<GenericFamily, Str>;

// MARK: FontAttrs -------------------------------------------------------------

struct FontAttrs {
    String family;
    FontWeight weight = FontWeight::REGULAR;
    FontStretch stretch = FontStretch::NORMAL;
    FontStyle style = FontStyle::NORMAL;
    Monospace monospace = Monospace::NO;
};

} // namespace Karm::Text
