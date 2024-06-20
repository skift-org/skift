#pragma once

#include <karm-base/panic.h>
#include <karm-base/std.h>
#include <karm-io/emit.h>

namespace Vaev {

// 2. MARK: Box Layout Modes: the display property
// https://www.w3.org/TR/css-display-3/#the-display-properties

struct Display {
    enum struct Box : u8 {
        CONTENT,
        NONE,
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
        RUBY_BASE,
        RUBY_TEXT,
        RUBY_BASE_CONTAINER,
        RUBY_TEXT_CONTAINER,
    };

    using enum Internal;

    enum struct Inside : u8 {
        FLOW,
        FLOW_ROOT,
        FLEX,
        GRID,
        RUBY,
        TABLE,
        MATH,
    };

    using enum Inside;

    enum struct Outside : u8 {
        BLOCK,
        INLINE,
        RUN_IN,
    };

    using enum Outside;

    enum struct Item : u8 {
        NO,
        YES
    };

    enum struct Type : u8 {
        DEFAULT,
        INTERNAL,
        BOX
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

    Display(Box box)
        : _type(Type::BOX), _box(box) {}

    Display(Internal internal)
        : _type(Type::INTERNAL), _internal(internal) {}

    Display(Inside inside, Outside outside, Item item = Item::NO)
        : _type(Type::DEFAULT), _inside(inside), _outside(outside), _item(item) {}

    bool is(Type type) const {
        return _type == type;
    }

    Type type() const {
        return _type;
    }

    bool operator==(Type type) const {
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

    bool operator==(Internal internal) const {
        return is(Type::INTERNAL) and _internal == internal;
    }

    void repr(Io::Emit &e) const {
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
        }
    }
};

} // namespace Vaev
