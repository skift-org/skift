export module Karm.Ui:text;

import Karm.Core;
import Karm.App;

namespace Karm::Ui {

bool _isWord(Rune r) {
    return isAsciiAlphaNum(r) or r == '_';
}

export struct TextAction {
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

    TextAction(_Op op, Rune rune = 0) : op(op), rune(rune) {}

    static Opt<TextAction> fromEvent(App::Event& e) {
        if (auto te = e.is<App::TypeEvent>()) {
            return TextAction{TYPE, te->rune};
        } else if (
            auto ke = e.is<App::KeyboardEvent>();
            ke and (ke->type == App::KeyboardEvent::PRESS or ke->type == App::KeyboardEvent::REPEATE)
        ) {
            bool shift = App::match(ke->mods, App::KeyMod::SHIFT);
            bool ctrl = App::match(ke->mods, App::KeyMod::CTRL);
            bool alt = App::match(ke->mods, App::KeyMod::ALT);
            bool nomod = not(shift or ctrl or alt);
            bool optionalyShift = (nomod or shift);

            if (ke->key == App::Key::HOME and ctrl and shift)
                return SELECT_START;
            else if (ke->key == App::Key::END and ctrl and shift)
                return SELECT_END;
            else if (ke->key == App::Key::LEFT and ctrl and shift)
                return SELECT_PREV_WORD;
            else if (ke->key == App::Key::RIGHT and ctrl and shift)
                return SELECT_NEXT_WORD;
            else if (ke->key == App::Key::Z and ctrl and shift)
                return REDO;

            else if (ke->key == App::Key::LEFT and shift)
                return SELECT_PREV;
            else if (ke->key == App::Key::RIGHT and shift)
                return SELECT_NEXT;
            else if (ke->key == App::Key::UP and shift)
                return SELECT_UP;
            else if (ke->key == App::Key::DOWN and shift)
                return SELECT_DOWN;
            else if (ke->key == App::Key::HOME and shift)
                return SELECT_LINE_START;
            else if (ke->key == App::Key::END and shift)
                return SELECT_LINE_END;

            else if (ke->key == App::Key::BKSPC and ctrl)
                return DELETE_PREV_WORD;
            else if (ke->key == App::Key::DELETE and ctrl)
                return DELETE_NEXT_WORD;
            else if (ke->key == App::Key::LEFT and ctrl)
                return MOVE_PREV_WORD;
            else if (ke->key == App::Key::RIGHT and ctrl)
                return MOVE_NEXT_WORD;
            else if (ke->key == App::Key::HOME and ctrl)
                return MOVE_START;
            else if (ke->key == App::Key::END and ctrl)
                return MOVE_END;
            else if (ke->key == App::Key::A and ctrl)
                return SELECT_ALL;
            else if (ke->key == App::Key::C and ctrl)
                return COPY;
            else if (ke->key == App::Key::X and ctrl)
                return CUT;
            else if (ke->key == App::Key::V and ctrl)
                return PASTE;
            else if (ke->key == App::Key::Z and ctrl)
                return UNDO;

            else if (ke->key == App::Key::LEFT and nomod)
                return MOVE_PREV;
            else if (ke->key == App::Key::RIGHT and nomod)
                return MOVE_NEXT;
            else if (ke->key == App::Key::UP and nomod)
                return MOVE_UP;
            else if (ke->key == App::Key::DOWN and nomod)
                return MOVE_DOWN;
            else if (ke->key == App::Key::HOME and nomod)
                return MOVE_LINE_START;
            else if (ke->key == App::Key::END and nomod)
                return MOVE_LINE_END;
            else if (ke->key == App::Key::ENTER and nomod)
                return NEWLINE;

            else if (ke->key == App::Key::BKSPC and optionalyShift)
                return BACKSPACE;
            else if (ke->key == App::Key::DELETE and optionalyShift)
                return DELETE;
        }

        return NONE;
    }
};

export struct TextModel {
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

    TextModel(Str text = "") {
        for (auto r : iterRunes(text))
            _buf.pushBack(r);

        _cur.head = _buf.len();
        _cur.tail = _buf.len();
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

    void _do(Record& r) {
        switch (r.op) {
        case INSERT:
            _buf.insert(r.pos, r.rune);
            break;

        case MOVE:
            _cur.head = r.pos;
            _cur.tail = r.pos;
            break;

        case SELECT:
            _cur.head = r.pos;
            break;

        case DELETE:
            auto start = min(_cur.head, r.pos);
            auto end = max(_cur.head, r.pos);
            auto slice = sub(_buf, start, end);
            r.buf = slice;
            r.pos = start;

            _cur.head = start;
            _cur.tail = start;

            _buf.removeRange(start, end - start);
            break;
        }
    }

    void _undo(Record& r) {
        switch (r.op) {
        case INSERT:
            _buf.removeAt(r.pos);
            break;

        case MOVE:
        case SELECT:
            break;

        case DELETE:
            _buf.insertMany(r.pos, r.buf);
            break;
        }

        _cur = r.cur;
    }

    void _push(Op op, usize pos, Rune rune) {
        if (_index != _records.len())
            _records.trunc(_index);

        auto& record = _records.emplaceBack();
        record.op = op;
        record.pos = min(pos, _buf.len());
        record.rune = rune;
        record.cur = _cur;
        record.group = _group;
        _do(record);

        _index++;
    }

    void _insertTo(usize pos, Rune rune) {
        _push(INSERT, pos, rune);
    }

    void _moveTo(usize pos) {
        _push(MOVE, pos, 0);
    }

    void _selectTo(usize pos) {
        _push(SELECT, pos, 0);
    }

    void _deleteTo(usize pos) {
        _push(DELETE, pos, 0);
    }

    bool _deleteSel() {
        if (not _cur.open())
            return false;

        _deleteTo(_cur.tail);
        _cur.head = min(_cur.head, _cur.tail);
        _cur.tail = _cur.head;

        return true;
    }

    void _checkpoint() {
        _group++;
    }

    // MARK: Movements

    usize _prev(usize pos) const {
        if (pos == 0)
            return 0;

        return pos - 1;
    }

    usize _next(usize pos) const {
        if (pos == _buf.len())
            return pos;

        return pos + 1;
    }

    usize _up(usize pos) const {
        auto off = pos - _lineStart(pos);
        auto prevLine = _lineStart(_prevLine(pos));
        return min(prevLine + off, _lineEnd(prevLine));
    }

    usize _down(usize pos) const {
        auto off = pos - _lineStart(pos);
        auto nextLine = _lineStart(_nextLine(pos));
        return min(nextLine + off, _lineEnd(nextLine));
    }

    usize _prevWord(usize pos) const {
        auto startedFromWord = pos != 0 and _isWord(_buf[pos - 1]);

        if (not startedFromWord)
            while (pos != 0 and not _isWord(_buf[pos - 1]))
                pos--;

        while (pos != 0 and _isWord(_buf[pos - 1]))
            pos--;

        return pos;
    }

    usize _nextWord(usize pos) const {
        auto startedFromWord = pos != _buf.len() and _isWord(_buf[pos]);

        if (not startedFromWord)
            while (pos != _buf.len() and not _isWord(_buf[pos]))
                pos++;

        while (pos != _buf.len() and _isWord(_buf[pos]))
            pos++;

        return pos;
    }

    usize _lineStart(usize pos) const {
        while (pos != 0 and _buf[pos - 1] != '\n')
            pos--;

        return pos;
    }

    usize _lineEnd(usize pos) const {
        while (pos != _buf.len() and _buf[pos] != '\n')
            pos++;

        return pos;
    }

    usize _prevLine(usize pos) const {
        return _prev(_lineStart(pos));
    }

    usize _nextLine(usize pos) const {
        return _next(_lineEnd(pos));
    }

    usize _textStart(usize = 0) const {
        return 0;
    }

    usize _textEnd(usize = 0) const {
        return _buf.len();
    }

    // MARK: Commands

    void insert(Rune rune) {
        _checkpoint();
        _deleteSel();
        _insertTo(_cur.head, rune);
        _moveTo(_next(_cur.head));
    }

    void newline() {
        insert('\n');
    }

    void backspace() {
        _checkpoint();
        if (not _deleteSel())
            _deleteTo(_prev(_cur.head));
    }

    void delete_() {
        _checkpoint();
        if (not _deleteSel())
            _deleteTo(_next(_cur.head));
    }

    void deletePrevWord() {
        _checkpoint();
        if (not _deleteSel())
            _deleteTo(_prevWord(_cur.head));
    }

    void deleteNextWord() {
        _checkpoint();
        if (not _deleteSel())
            _deleteTo(_nextWord(_cur.head));
    }

    void movePrev() {
        if (_cur.open())
            _moveTo(_cur.head);
        else
            _moveTo(_prev(_cur.head));
    }

    void moveNext() {
        if (_cur.open())
            _moveTo(_cur.head);
        else
            _moveTo(_next(_cur.head));
    }

    void moveUp() {
        _moveTo(_up(_cur.head));
    }

    void moveDown() {
        _moveTo(_down(_cur.head));
    }

    void movePrevWord() {
        _moveTo(_prevWord(_cur.head));
    }

    void moveNextWord() {
        _moveTo(_nextWord(_cur.head));
    }

    void moveLineStart() {
        _moveTo(_lineStart(_cur.head));
    }

    void moveLineEnd() {
        _moveTo(_lineEnd(_cur.head));
    }

    void moveStart() {
        _moveTo(_textStart(_cur.head));
    }

    void moveEnd() {
        _moveTo(_textEnd(_cur.head));
    }

    void selectPrev() {
        _selectTo(_prev(_cur.head));
    }

    void selectNext() {
        _selectTo(_next(_cur.head));
    }

    void selectUp() {
        _selectTo(_up(_cur.head));
    }

    void selectDown() {
        _selectTo(_down(_cur.head));
    }

    void selectPrevWord() {
        _selectTo(_prevWord(_cur.head));
    }

    void selectNextWord() {
        _selectTo(_nextWord(_cur.head));
    }

    void selectLineStart() {
        _selectTo(_lineStart(_cur.head));
    }

    void selectLineEnd() {
        _selectTo(_lineEnd(_cur.head));
    }

    void selectStart() {
        _selectTo(_textStart(_cur.head));
    }

    void selectEnd() {
        _selectTo(_textEnd(_cur.head));
    }

    void selectAll() {
        _moveTo(_textStart(_cur.head));
        _selectTo(_textEnd(_cur.head));
    }

    String copy() {
        StringBuilder sb;
        sb.append(sub(_buf, _cur.head, _cur.tail));
        return sb.take();
    }

    String cut() {
        _checkpoint();
        auto str = copy();
        _deleteSel();
        return str;
    }

    void paste(Str text) {
        _checkpoint();
        _deleteSel();
        for (auto rune : iterRunes(text))
            _insertTo(_cur.head, rune);
    }

    void undo() {
        if (_index == 0)
            return;

        auto group = _records[_index - 1].group;
        while (_index != 0 and _records[_index - 1].group == group) {
            _undo(_records[_index - 1]);
            _index--;
        }
    }

    bool canUndo() const { return _index > 0; }

    void redo() {
        if (_index == _records.len())
            return;

        auto group = _records[_index].group;
        while (_index != _records.len() and _records[_index].group == group) {
            _do(_records[_index]);
            _index++;
        }
    }

    bool canRedo() const { return _index < _records.len(); }

    void flush() {
        _records.clear();
    }

    void reduce(TextAction const& a) {
        switch (a.op) {
        case TextAction::TYPE:
            insert(a.rune);
            break;

        case TextAction::NEWLINE:
            newline();
            break;

        case TextAction::BACKSPACE:
            backspace();
            break;

        case TextAction::DELETE:
            delete_();
            break;

        case TextAction::DELETE_PREV_WORD:
            deletePrevWord();
            break;

        case TextAction::DELETE_NEXT_WORD:
            deleteNextWord();
            break;

        case TextAction::MOVE_PREV:
            movePrev();
            break;

        case TextAction::MOVE_NEXT:
            moveNext();
            break;

        case TextAction::MOVE_UP:
            moveUp();
            break;

        case TextAction::MOVE_DOWN:
            moveDown();
            break;

        case TextAction::MOVE_PREV_WORD:
            movePrevWord();
            break;

        case TextAction::MOVE_NEXT_WORD:
            moveNextWord();
            break;

        case TextAction::MOVE_LINE_START:
            moveLineStart();
            break;

        case TextAction::MOVE_LINE_END:
            moveLineEnd();
            break;

        case TextAction::MOVE_START:
            moveStart();
            break;

        case TextAction::MOVE_END:
            moveEnd();
            break;

        case TextAction::SELECT_PREV:
            selectPrev();
            break;

        case TextAction::SELECT_NEXT:
            selectNext();
            break;

        case TextAction::SELECT_UP:
            selectUp();
            break;

        case TextAction::SELECT_DOWN:
            selectDown();
            break;

        case TextAction::SELECT_PREV_WORD:
            selectPrevWord();
            break;

        case TextAction::SELECT_NEXT_WORD:
            selectNextWord();
            break;

        case TextAction::SELECT_LINE_START:
            selectLineStart();
            break;

        case TextAction::SELECT_LINE_END:
            selectLineEnd();
            break;

        case TextAction::SELECT_START:
            selectStart();
            break;

        case TextAction::SELECT_END:
            selectEnd();
            break;

        case TextAction::SELECT_ALL:
            selectAll();
            break;

        case TextAction::UNDO:
            undo();
            break;

        case TextAction::REDO:
            redo();
            break;

        default:
            break;
        }
    }

    bool dirty() const {
        for (auto& r : _records)
            if (r.op == INSERT or r.op == DELETE)
                return true;
        return false;
    }
};

} // namespace Karm::Ui
