module;

#include <karm-core/macros.h>

export module Karm.Core:regex;

import :base.flags;
import :base.rc;
import :base.res;
import :io.emit;
import :io.expr;
import :io.sscan;

namespace Karm::Regex {

enum struct Option {
    GLOBAL = 1 << 0,      // All matches (don't return after first match)
    MULTI_LINE = 1 << 1,  // ^ and $ match start/end of line
    INSENSITIVE = 1 << 2, // Case-insensitive match
    STICKY = 1 << 3,      // Force the pattern to only match consecutive matches from where the previous match ended.
    UNICODE = 1 << 4,     // Match with full Unicode
    VNICODE = 1 << 5,
    SINGLE_LINE = 1 << 6, // Dot matches newline characters
};

enum struct Flavor {
    ECMA_SCRIPT,
    _LEN,
};

// MARK: Matcher ---------------------------------------------------------------

struct Match {
    usize end;
};

struct Matcher {
    virtual ~Matcher() = default;

    virtual Opt<Match> match(Str input, usize curr) const = 0;

    virtual void repr(Io::Emit& e) const = 0;
};

struct EitherMatcher : Matcher {
    Rc<Matcher> _lhs;
    Rc<Matcher> _rhs;

    EitherMatcher(Rc<Matcher> lhs, Rc<Matcher> rhs)
        : _lhs(lhs), _rhs(rhs) {}

    Opt<Match> match(Str input, usize curr) const override {
        auto lhsMatch = _lhs->match(input, curr);
        auto rhsMatch = _rhs->match(input, curr);
        if (not rhsMatch)
            return lhsMatch;
        if (not lhsMatch)
            return rhsMatch;
        if (lhsMatch->end > rhsMatch->end)
            return lhsMatch;
        return rhsMatch;
    }

    void repr(Io::Emit& e) const override {
        e("(either {} {})", _lhs, _rhs);
    }
};

struct ChainMatcher : Matcher {
    Rc<Matcher> _lhs;
    Rc<Matcher> _rhs;

    ChainMatcher(Rc<Matcher> lhs, Rc<Matcher> rhs)
        : _lhs(lhs), _rhs(rhs) {}

    Opt<Match> match(Str input, usize curr) const override {
        auto lhsMatch = _lhs->match(input, curr);
        if (not lhsMatch)
            return NONE;
        return _rhs->match(input, lhsMatch->end);
    }

    void repr(Io::Emit& e) const override {
        e("(chain {} {})", _lhs, _rhs);
    }
};

struct NopMatcher : Matcher {
    Opt<Match> match(Str, usize curr) const override {
        return Match{
            curr,
        };
    }

    void repr(Io::Emit& e) const override {
        e("(nop)");
    }
};

struct AtomMatcher : Matcher {
    String _data;

    AtomMatcher(Rune rune) {
        StringBuilder sb;
        sb.append(rune);
        _data = sb.take();
    }

    AtomMatcher(String data)
        : _data(data) {}

    Opt<Match> match(Str input, usize curr) const override {
        Str s = sub(input, curr, curr + _data.len());
        if (s != _data)
            return NONE;
        return Match{
            curr + _data.len(),
        };
    }

    void repr(Io::Emit& e) const override {
        e("(atom {#})", _data);
    }
};

struct DotMatcher : Matcher {
    Opt<Match> match(Str input, usize curr) const override {
        auto rune = iterRunes(Str{next(input, curr)}).next();
        if (not rune)
            return NONE;
        if (rune == U'\n' or rune == U'\r' or rune == U'\u2028' or rune == U'\u2029')
            return NONE;

        return Match{
            curr + Utf8::runeLen(*rune)
        };
    }

    void repr(Io::Emit& e) const override {
        e("(dot)");
    }
};

struct Quantifier {
    u64 min;
    Opt<u64> max;

    void repr(Io::Emit& e) const {
        e("{{{},{}}}", min, max);
    }
};

struct QuantifierMatcher : Matcher {
    Quantifier _quant;
    Rc<Matcher> _matcher;

    QuantifierMatcher(Quantifier quant, Rc<Matcher> matcher)
        : _quant(quant), _matcher(matcher) {}

    Opt<Match> match(Str input, usize curr) const override {
        for (usize i = 0; i < _quant.max.unwrapOr(Limits<u64>::MAX); i++) {
            auto match = _matcher->match(input, curr);
            if (not match) {
                if (i < _quant.min)
                    return NONE;
                return Match{curr};
            }
            curr = match->end;
        }

        return Match{curr};
    }

    void repr(Io::Emit& e) const override {
        e("(quantifier {} {})", _quant, _matcher);
    }
};

enum struct Assertion {
    INPUT_BOUNDARY_BEGIN,
    INPUT_BOUNDARY_END,
    WORD_BOUNDARY,
    NON_WORD_BOUNDARY,

    _LEN
};

struct AssertionMatcher : Matcher {
    Assertion _assert;

    AssertionMatcher(Assertion assert) : _assert(assert) {}

    Opt<Match> match(Str input, usize curr) const override {
        switch (_assert) {
        case Assertion::INPUT_BOUNDARY_BEGIN:
            if (curr != 0)
                return NONE;
            break;
        case Assertion::INPUT_BOUNDARY_END:
            if (curr != input.len())
                return NONE;
            break;
        case Assertion::WORD_BOUNDARY:
            notImplemented();
            break;
        case Assertion::NON_WORD_BOUNDARY:
            notImplemented();
            break;
        default:
            unreachable();
        }

        return Match{curr};
    }

    void repr(Io::Emit& e) const override {
        e("(assertion {} {})", _assert);
    }
};

// MARK: Parser ----------------------------------------------------------------
// Following ECMA Script's patterns syntax (see https://tc39.es/ecma262/#sec-patterns)

// https://tc39.es/ecma262/#prod-SyntaxCharacter
static auto RE_SYNTAX_CHARACTER =
    Re::single('^', '$', '\\', '.', '*', '+', '?', '(', ')', '[', ']', '{', '}', '|');

// https://tc39.es/ecma262/#prod-PatternCharacter
static auto RE_PATTERN_CHARACTER =
    Re::negate(RE_SYNTAX_CHARACTER);

static Res<Quantifier> _parseQuantifier(Io::SScan& s) {
    if (s.skip("*"))
        return Ok(Quantifier{0, NONE});
    else if (s.skip("+"))
        return Ok(Quantifier{1, NONE});
    else if (s.skip("?"))
        return Ok(Quantifier{0, 1});
    else {
        return Error::invalidData("expected quantifier");
    }
}

// https://tc39.es/ecma262/#prod-AtomEscape
static Res<Rc<Matcher>> _parseAtomEscape(Io::SScan& s) {
    // https://tc39.es/ecma262/#prod-ControlEscape
    if (s.skip("t")) {
        return Ok(makeRc<AtomMatcher>('\t'));
    } else if (s.skip("n")) {
        return Ok(makeRc<AtomMatcher>('\n'));
    } else if (s.skip("v")) {
        return Ok(makeRc<AtomMatcher>('\v'));
    } else if (s.skip("f")) {
        return Ok(makeRc<AtomMatcher>('\f'));
    } else if (s.skip("r")) {
        return Ok(makeRc<AtomMatcher>('\r'));
    }

    // https://tc39.es/ecma262/#prod-IdentityEscape
    else if (s.ahead(RE_SYNTAX_CHARACTER)) {
        return Ok(makeRc<AtomMatcher>(s.next()));
    } else {
        return Error::invalidData("expected atom escape");
    }
}

static Res<Rc<Matcher>> _parseAtom(Io::SScan& s) {
    if (auto chr = s.token(RE_PATTERN_CHARACTER)) {
        return Ok(makeRc<AtomMatcher>(chr));
    } else if (s.skip(".")) {
        return Ok(makeRc<DotMatcher>());
    } else if (s.skip("\\")) {
        return _parseAtomEscape(s);
    } else {
        return Error::invalidData("expected atom");
    }
}

static Res<Rc<Matcher>> _parseAssertion(Io::SScan& s) {
    if (s.skip("^"))
        return Ok(makeRc<AssertionMatcher>(Assertion::INPUT_BOUNDARY_BEGIN));
    else if (s.skip("$"))
        return Ok(makeRc<AssertionMatcher>(Assertion::INPUT_BOUNDARY_END));
    else if (s.skip("\\b"))
        return Ok(makeRc<AssertionMatcher>(Assertion::WORD_BOUNDARY));
    else if (s.skip("\\B"))
        return Ok(makeRc<AssertionMatcher>(Assertion::NON_WORD_BOUNDARY));
    else
        return Error::invalidData("expected assertion");
}

static Res<Rc<Matcher>> _parseTerm(Io::SScan& s) {
    if (auto assert = _parseAssertion(s)) {
        return Ok(assert.take());
    } else if (auto atom = _parseAtom(s)) {
        auto quant = _parseQuantifier(s);
        if (quant)
            return Ok(makeRc<QuantifierMatcher>(quant.take(), atom.take()));
        return Ok(atom.take());
    } else {
        return Error::invalidData("expected term");
    }
}

static Res<Rc<Matcher>> _parseAlternative(Io::SScan& s) {
    auto lhs = _parseTerm(s).ok();

    if (not lhs)
        return Ok(makeRc<NopMatcher>());

    while (auto rhs = _parseTerm(s).ok())
        lhs = makeRc<ChainMatcher>(lhs.take(), rhs.take());

    return Ok(lhs.take());
}

static Res<Rc<Matcher>> _parseDisjunction(Io::SScan& s) {
    auto lhs = try$(_parseAlternative(s));
    if (not s.skip('|'))
        return Ok(lhs);
    auto rhs = try$(_parseDisjunction(s));
    return Ok(makeRc<EitherMatcher>(lhs, rhs));
}

static Res<Rc<Matcher>> _parsePattern(Io::SScan& s) {
    return _parseDisjunction(s);
}

// MARK: Regex -----------------------------------------------------------------

export struct Regex {
    Rc<Matcher> _matcher;
    Flags<Option> _options = {};

    static Regex from(Str pattern, Flags<Option> options = {}, Flavor flavor = Flavor::ECMA_SCRIPT) {
        if (flavor != Flavor::ECMA_SCRIPT)
            panic("only ecma script flavored regex supported");
        Io::SScan s = pattern;
        return {
            _parsePattern(s)
                .unwrap("invalid regex pattern"),
            options,
        };
    }

    static Regex verbatim(Str text) {
        return {
            makeRc<AtomMatcher>(text),
        };
    }

    Opt<Match> match(Str str) const {
        return _matcher->match(str, 0);
    }

    Opt<Match> wholeMatch(Str str) const {
        auto match = _matcher->match(str, 0);
        if (not match)
            return NONE;
        if (str.len() == match->end)
            return match;
        return NONE;
    }

    bool contains(Str str) const {
        return _matcher->match(str, 0).has();
    }

    void repr(Io::Emit& e) const {
        _matcher->repr(e);
    }
};

} // namespace Karm::Regex

export constexpr Karm::Regex::Regex operator""_regex(char const* str, Karm::usize len) {
    return Karm::Regex::Regex::from(Karm::Str{str, len});
}
