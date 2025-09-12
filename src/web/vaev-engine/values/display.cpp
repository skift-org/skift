module;

#include <karm-core/macros.h>

export module Vaev.Engine:values.display;

import Karm.Core;

import :css;
import :values.base;

using namespace Karm;

namespace Vaev {

// 2. MARK: Box Layout Modes: the display property
// https://www.w3.org/TR/css-display-3/#the-display-properties

export struct Display {
    enum struct Box : u8 {
        CONTENTS,
        NONE,

        _LEN0,
    };

    using enum Box;

    enum struct Internal : u8 {
        TABLE_ROW_GROUP,
        TABLE_HEADER_GROUP,
        TABLE_FOOTER_GROUP,
        TABLE_ROW,
        TABLE_CELL,
        TABLE_COLUMN_GROUP,
        TABLE_COLUMN,
        TABLE_CAPTION,
        TABLE_BOX,
        RUBY_BASE,
        RUBY_TEXT,
        RUBY_BASE_CONTAINER,
        RUBY_TEXT_CONTAINER,

        _LEN1,
    };

    bool isHeadBodyFootOrRow() {
        return (
            *this == TABLE_HEADER_GROUP or
            *this == TABLE_ROW_GROUP or
            *this == TABLE_FOOTER_GROUP or
            *this == TABLE_ROW
        );
    }

    bool isHeadBodyFootRowOrColGroup() {
        return (
            isHeadBodyFootOrRow() or
            *this == TABLE_COLUMN_GROUP
        );
    }

    using enum Internal;

    enum struct Inside : u8 {
        FLOW,
        FLOW_ROOT,
        FLEX,
        GRID,
        RUBY,
        TABLE,
        MATH,

        _LEN2,
    };

    using enum Inside;

    enum struct Outside : u8 {
        BLOCK,
        INLINE,
        RUN_IN,

        _LEN3,
    };

    using enum Outside;

    enum struct Item : u8 {
        NO,
        YES,

        _LEN4,
    };

    enum struct Type : u8 {
        DEFAULT,
        INTERNAL,
        BOX,

        _LEN5,
    };

    using enum Type;

    Type _type;

    union {
        struct {
            Inside _inside;
            Outside _outside;
            Item _item;
        };

        Box _box;
        Internal _internal;
    };

    constexpr Display()
        : Display(Display::FLOW, Display::INLINE) {}

    constexpr Display(Box box)
        : _type(Type::BOX), _box(box) {}

    constexpr Display(Internal internal)
        : _type(Type::INTERNAL), _internal(internal) {}

    constexpr Display(Inside inside, Outside outside, Item item = Item::NO)
        : _type(Type::DEFAULT), _inside(inside), _outside(outside), _item(item) {}

    constexpr bool is(Type type) const {
        return _type == type;
    }

    constexpr Type type() const {
        return _type;
    }

    constexpr bool operator==(Type type) const {
        return _type == type;
    }

    Inside inside() const {
        if (not is(Type::DEFAULT))
            panic("invalid display type");
        return _inside;
    }

    bool operator==(Inside inside) const {
        return is(Type::DEFAULT) and _inside == inside;
    }

    Outside outside() const {
        if (not is(Type::DEFAULT))
            panic("invalid display type");
        return _outside;
    }

    bool operator==(Outside outside) const {
        return is(Type::DEFAULT) and _outside == outside;
    }

    Item item() const {
        if (not is(Type::DEFAULT))
            panic("invalid display type");
        return _item;
    }

    bool operator==(Item item) const {
        return is(Type::DEFAULT) and _item == item;
    }

    Box box() const {
        if (not is(Type::BOX))
            panic("invalid display type");
        return _box;
    }

    bool operator==(Box box) const {
        return is(Type::BOX) and _box == box;
    }

    Internal internal() const {
        if (not is(Type::INTERNAL))
            panic("invalid display type");
        return _internal;
    }

    bool operator==(Internal const& internal) const {
        return is(Type::INTERNAL) and _internal == internal;
    }

    bool operator==(Display const& other) const {
        if (_type != other._type)
            return false;

        switch (_type) {
        case Type::DEFAULT:
            return _inside == other._inside and _outside == other._outside and _item == other._item;

        case Type::INTERNAL:
            return _internal == other._internal;

        case Type::BOX:
            return _box == other._box;

        default:
            unreachable();
        }

        return false;
    }

    void repr(Io::Emit& e) const {
        switch (_type) {
        case Type::DEFAULT:
            e("(display inside: {}, outside: {}, item: {})", _inside, _outside, _item);
            break;

        case Type::INTERNAL:
            e("(display internal: {})", _internal);
            break;

        case Type::BOX:
            e("(display box: {})", _box);
            break;

        default:
            unreachable();
        }
    }

    always_inline bool isTableTrack() const {
        if (not is(Type::INTERNAL))
            return false;
        return _internal == TABLE_ROW or _internal == TABLE_COLUMN;
    }

    always_inline bool isTableTrackGroup() const {
        if (not is(Type::INTERNAL))
            return false;
        return _internal == TABLE_ROW_GROUP or _internal == TABLE_HEADER_GROUP or
               _internal == TABLE_FOOTER_GROUP or _internal == TABLE_COLUMN_GROUP;
    }

    always_inline bool isProperTableChild() const {
        if (not is(Type::INTERNAL))
            return false;
        return _internal == TABLE_CAPTION or isTableTrack() or isTableTrackGroup();
    }

    bool isTableInternal() const {
        if (type() != INTERNAL)
            return false;

        // https://www.w3.org/TR/css-display-3/#internal-table-element
        static Array constexpr const INTERNAL_TABLE = {
            TABLE_ROW_GROUP,
            TABLE_HEADER_GROUP,
            TABLE_FOOTER_GROUP,
            TABLE_ROW,
            TABLE_CELL,
            TABLE_COLUMN_GROUP,
            TABLE_COLUMN,
            TABLE_BOX
        };
        return contains(INTERNAL_TABLE, _internal);
    }

    // https://www.w3.org/TR/css-display-3/#blockify
    Display blockify() {
        if (_type == BOX)
            return *this;

        if (_type == INTERNAL) {
            // If a layout-internal box is blockified, its inner display type converts to flow so that it becomes
            // a block container.
            // FIXME: in our representation, layout-internal does not have an inner display property
            panic("cannot blockify layout-internal display");
        }

        if (_outside == BLOCK)
            return *this;

        if (_inside == FLOW_ROOT) {
            return {
                FLOW,
                BLOCK,
                _item,
            };
        } else {
            return {
                _inside,
                BLOCK,
                _item,
            };
        }
    }

    // https://www.w3.org/TR/css-display-3/#inlinify
    Display inlinify() {
        if (_type != DEFAULT)
            return *this;

        if (_outside == INLINE)
            return *this;

        if (_inside == FLOW) {
            return {
                FLOW,
                BLOCK,
                _item,
            };
        } else {
            return {
                _inside,
                BLOCK,
                _item,
            };
        }
    }
};

export template <>
struct ValueParser<Display> {
    static Res<Display> _parseLegacyDisplay(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("inline-block"))) {
            return Ok(Display{Display::FLOW_ROOT, Display::INLINE});
        } else if (c.skip(Css::Token::ident("inline-table"))) {
            return Ok(Display{Display::TABLE, Display::INLINE});
        } else if (c.skip(Css::Token::ident("inline-flex"))) {
            return Ok(Display{Display::FLEX, Display::INLINE});
        } else if (c.skip(Css::Token::ident("inline-grid"))) {
            return Ok(Display{Display::GRID, Display::INLINE});
        }

        return Error::invalidData("expected legacy display value");
    }

    static Res<Display::Outside> _parseOutsideDisplay(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("block"))) {
            return Ok(Display::BLOCK);
        } else if (c.skip(Css::Token::ident("inline"))) {
            return Ok(Display::INLINE);
        } else if (c.skip(Css::Token::ident("run-in"))) {
            return Ok(Display::RUN_IN);
        }

        return Error::invalidData("expected outside value");
    }

    static Res<Display::Inside> _parseInsideDisplay(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("flow"))) {
            return Ok(Display::FLOW);
        } else if (c.skip(Css::Token::ident("flow-root"))) {
            return Ok(Display::FLOW_ROOT);
        } else if (c.skip(Css::Token::ident("flex"))) {
            return Ok(Display::FLEX);
        } else if (c.skip(Css::Token::ident("grid"))) {
            return Ok(Display::GRID);
        } else if (c.skip(Css::Token::ident("ruby"))) {
            return Ok(Display::RUBY);
        } else if (c.skip(Css::Token::ident("table"))) {
            return Ok(Display::TABLE);
        } else if (c.skip(Css::Token::ident("math"))) {
            return Ok(Display::MATH);
        }

        return Error::invalidData("expected inside value");
    }

    static Res<Display> _parseInternalDisplay(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("table-row-group"))) {
            return Ok(Display::TABLE_ROW_GROUP);
        } else if (c.skip(Css::Token::ident("table-header-group"))) {
            return Ok(Display::TABLE_HEADER_GROUP);
        } else if (c.skip(Css::Token::ident("table-footer-group"))) {
            return Ok(Display::TABLE_FOOTER_GROUP);
        } else if (c.skip(Css::Token::ident("table-row"))) {
            return Ok(Display::TABLE_ROW);
        } else if (c.skip(Css::Token::ident("table-cell"))) {
            return Ok(Display::TABLE_CELL);
        } else if (c.skip(Css::Token::ident("table-column-group"))) {
            return Ok(Display::TABLE_COLUMN_GROUP);
        } else if (c.skip(Css::Token::ident("table-column"))) {
            return Ok(Display::TABLE_COLUMN);
        } else if (c.skip(Css::Token::ident("table-caption"))) {
            return Ok(Display::TABLE_CAPTION);
        } else if (c.skip(Css::Token::ident("ruby-base"))) {
            return Ok(Display::RUBY_BASE);
        } else if (c.skip(Css::Token::ident("ruby-text"))) {
            return Ok(Display::RUBY_TEXT);
        } else if (c.skip(Css::Token::ident("ruby-base-container"))) {
            return Ok(Display::RUBY_BASE_CONTAINER);
        } else if (c.skip(Css::Token::ident("ruby-text-container"))) {
            return Ok(Display::RUBY_TEXT_CONTAINER);
        }

        return Error::invalidData("expected internal value");
    }

    static Res<Display> _parseBoxDisplay(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("contents"))) {
            return Ok(Display::CONTENTS);
        } else if (c.skip(Css::Token::ident("none"))) {
            return Ok(Display::NONE);
        }

        return Error::invalidData("expected box value");
    }

    static Res<Display> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (auto legacy = _parseLegacyDisplay(c))
            return legacy;

        if (auto box = _parseBoxDisplay(c))
            return box;

        if (auto internal = _parseInternalDisplay(c))
            return internal;

        auto inside = _parseInsideDisplay(c);
        auto outside = _parseOutsideDisplay(c);
        auto item = c.skip(Css::Token::ident("list-item"))
                        ? Display::Item::YES
                        : Display::Item::NO;

        if (not inside and not outside and not bool(item))
            return Error::invalidData("expected display value");

        return Ok(Display{
            inside.unwrapOr(Display::FLOW),
            outside.unwrapOr(Display::BLOCK),
            item,
        });
    }
};

} // namespace Vaev
