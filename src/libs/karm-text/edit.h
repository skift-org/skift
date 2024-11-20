#pragma once

#include <karm-app/inputs.h>

namespace Karm::Text {

struct Action {
    enum struct _Op {
        TYPE,
        NEWLINE,
        BACKSPACE,

        DELETE,
        DELETE_PREV_WORD,
        DELETE_NEXT_WORD,

        MOVE_PREV,
        MOVE_NEXT,
        MOVE_UP,
        MOVE_DOWN,
        MOVE_PREV_WORD,
        MOVE_NEXT_WORD,
        MOVE_LINE_START,
        MOVE_LINE_END,
        MOVE_START,
        MOVE_END,

        SELECT_PREV,
        SELECT_NEXT,
        SELECT_UP,
        SELECT_DOWN,
        SELECT_PREV_WORD,
        SELECT_NEXT_WORD,
        SELECT_LINE_START,
        SELECT_LINE_END,
        SELECT_START,
        SELECT_END,
        SELECT_ALL,

        COPY,
        CUT,
        PASTE,

        UNDO,
        REDO,

    };
    using enum _Op;

    _Op op;
    Rune rune = {};

    Action(_Op op, Rune rune = 0) : op(op), rune(rune) {}

    static Opt<Action> fromEvent(App::Event &e);
};

struct Model {
    static Str DUMMY_CLIPBOARD(Str = "") { return ""; }

    enum Op {
        INSERT,
        MOVE,
        SELECT,
        DELETE,
    };

    struct Cur {
        usize head;
        usize tail;

        bool open() const {
            return head != tail;
        }
    };

    struct Record {
        Op op;
        usize pos;
        Rune rune;
        Cur cur;
        Vec<Rune> buf;
        usize group;
    };

    Vec<Rune> _buf;
    Vec<Record> _records;
    usize _index{};
    usize _group{};
    Cur _cur{};

    Model(Str text = "") {
        for (auto r : iterRunes(text))
            _buf.pushBack(r);
    }

    Slice<Rune> runes() const {
        return _buf;
    }

    String string() const {
        StringBuilder sb;
        sb.append(runes());
        return sb.take();
    }

    void load(Str text) {
        for (auto r : iterRunes(text))
            _buf.pushBack(r);
    }

    // MARK: Operations

    void _do(Record &r);

    void _undo(Record &r);

    void _push(Op op, usize pos, Rune rune);

    void _insertTo(usize pos, Rune rune);

    void _moveTo(usize pos);

    void _selectTo(usize pos);

    void _deleteTo(usize pos);

    bool _deleteSel();

    void _checkpoint();

    // MARK: Movements

    usize _prev(usize pos) const;

    usize _next(usize pos) const;

    usize _up(usize pos) const;

    usize _down(usize pos) const;

    usize _prevWord(usize pos) const;

    usize _nextWord(usize pos) const;

    usize _lineStart(usize pos) const;

    usize _lineEnd(usize pos) const;

    usize _prevLine(usize pos) const;

    usize _nextLine(usize pos) const;

    usize _textStart(usize = 0) const;

    usize _textEnd(usize = 0) const;

    // MARK: Commands

    void insert(Rune rune);

    void newline();

    void backspace();

    void delete_();

    void deletePrevWord();

    void deleteNextWord();

    void movePrev();

    void moveNext();

    void moveUp();

    void moveDown();

    void movePrevWord();

    void moveNextWord();

    void moveLineStart();

    void moveLineEnd();

    void moveStart();

    void moveEnd();

    void selectPrev();

    void selectNext();

    void selectUp();

    void selectDown();

    void selectPrevWord();

    void selectNextWord();

    void selectLineStart();

    void selectLineEnd();

    void selectStart();

    void selectEnd();

    void selectAll();

    String copy();

    String cut();

    void paste(Str text);

    void undo();

    bool canUndo() const { return _index > 0; }

    void redo();

    bool canRedo() const { return _index < _records.len(); }

    void flush();

    void reduce(Action const &a);

    bool dirty() const {
        for (auto &r : _records)
            if (r.op == INSERT or r.op == DELETE)
                return true;
        return false;
    }
};

} // namespace Karm::Text
