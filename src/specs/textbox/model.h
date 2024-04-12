#pragma once

#include <karm-base/ctype.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-events/events.h>
#include <karm-sys/async.h>

namespace Textbox {

static bool _isWord(Rune r) {
    return isAsciiAlphaNum(r) or r == '_';
}

static bool _isSeparator(Rune r) {
    return not _isWord(r);
}

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

    // MARK: Operations

    void _do(Record &r) {
        switch (r.op) {
        case INSERT:
            _buf.insert(r.pos, r.rune);
            break;

        case MOVE:
            _cur.head = r.pos;
            _cur.tail = r.pos;
            break;

        case SELECT:
            _cur.tail = r.pos;
            break;

        case DELETE:
            auto start = min(_cur.head, r.pos);
            auto end = max(_cur.head, r.pos);
            auto slice = sub(_buf, start, end);
            r.buf = slice;
            _buf.removeRange(start, end - start);
            break;
        }
    }

    void _undo(Record &r) {
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

        auto &record = _records.emplaceBack();
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
        auto startedFromWord = _isWord(_buf[pos]);
        while (pos != 0 and (startedFromWord ? _isWord : _isSeparator)(_buf[pos - 1]))
            pos--;
        while (pos != 0 and (startedFromWord ? _isSeparator : _isWord)(_buf[pos - 1]))
            pos--;
        return pos;
    }

    usize _nextWord(usize pos) const {
        auto startedFromWord = _isWord(_buf[pos]);
        while (pos != 0 and (startedFromWord ? _isWord : _isSeparator)(_buf[pos]))
            pos++;
        while (pos != 0 and (startedFromWord ? _isSeparator : _isWord)(_buf[pos]))
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
    }

    void newline() {
        _checkpoint();
        _deleteSel();
        _insertTo(_cur.head, '\n');
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
        _moveTo(_prev(_cur.head));
    }

    void moveNext() {
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
        _selectTo(_prev(_cur.tail));
    }

    void selectNext() {
        _selectTo(_next(_cur.tail));
    }

    void selectUp() {
        _selectTo(_up(_cur.tail));
    }

    void selectDown() {
        _selectTo(_down(_cur.tail));
    }

    void selectPrevWord() {
        _selectTo(_prevWord(_cur.tail));
    }

    void selectNextWord() {
        _selectTo(_nextWord(_cur.tail));
    }

    void selectLineStart() {
        _selectTo(_lineStart(_cur.tail));
    }

    void selectLineEnd() {
        _selectTo(_lineEnd(_cur.tail));
    }

    void selectStart() {
        _selectTo(_textStart(_cur.tail));
    }

    void selectEnd() {
        _selectTo(_textEnd(_cur.tail));
    }

    void selectAll() {
        _selectTo(_textEnd(_cur.head));
        _selectTo(_textStart(_cur.head));
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
        for (auto rune : iterRunes(text)) {
            _insertTo(_cur.head, rune);
        }
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

    void redo() {
        if (_index == _records.len())
            return;

        auto group = _records[_index].group;
        while (_index != _records.len() and _records[_index].group == group) {
            _do(_records[_index]);
            _index++;
        }
    }

    void flush() {
        _records.clear();
    }

    void handle(Sys::Event &e, auto &clipboard = DUMMY_CLIPBOARD) {
        if (auto *te = e.is<Events::TypedEvent>()) {
            insert(te->codepoint);
            e.accept();
        } else if (auto *ke = e.is<Events::KeyboardEvent>()) {
            bool shift = !!(ke->mods & Events::Mod::ANY_SHIFT);
            bool ctrl = !!(ke->mods & Events::Mod::ANY_CTR);
            bool nomod = ke->mods == Events::Mod::NONE;
            bool optionalyShift = (ke->mods == Events::Mod::NONE or shift);

            if (ke->key == Events::Key::BKSPC and optionalyShift)
                backspace();
            else if (ke->key == Events::Key::DELETE and optionalyShift)
                delete_();
            else if (ke->key == Events::Key::BKSPC and ctrl)
                deletePrevWord();
            else if (ke->key == Events::Key::DELETE and ctrl)
                deleteNextWord();
            else if (ke->key == Events::Key::LEFT and nomod)
                movePrev();
            else if (ke->key == Events::Key::RIGHT and nomod)
                moveNext();
            else if (ke->key == Events::Key::UP and nomod)
                moveUp();
            else if (ke->key == Events::Key::DOWN and nomod)
                moveDown();
            else if (ke->key == Events::Key::LEFT and ctrl)
                movePrevWord();
            else if (ke->key == Events::Key::RIGHT and ctrl)
                moveNextWord();
            else if (ke->key == Events::Key::HOME and nomod)
                moveLineStart();
            else if (ke->key == Events::Key::END and nomod)
                moveLineEnd();
            else if (ke->key == Events::Key::HOME and ctrl)
                moveStart();
            else if (ke->key == Events::Key::END and ctrl)
                moveEnd();
            else if (ke->key == Events::Key::LEFT and shift)
                selectPrev();
            else if (ke->key == Events::Key::RIGHT and shift)
                selectNext();
            else if (ke->key == Events::Key::UP and shift)
                selectUp();
            else if (ke->key == Events::Key::DOWN and shift)
                selectDown();
            else if (ke->key == Events::Key::LEFT and ctrl and shift)
                selectPrevWord();
            else if (ke->key == Events::Key::RIGHT and ctrl and shift)
                selectNextWord();
            else if (ke->key == Events::Key::HOME and shift)
                selectLineStart();
            else if (ke->key == Events::Key::END and shift)
                selectLineEnd();
            else if (ke->key == Events::Key::HOME and ctrl and shift)
                selectStart();
            else if (ke->key == Events::Key::END and ctrl and shift)
                selectEnd();
            else if (ke->key == Events::Key::A and ctrl)
                selectAll();
            else if (ke->key == Events::Key::C and ctrl)
                clipboard(copy());
            else if (ke->key == Events::Key::X and ctrl)
                clipboard(cut());
            else if (ke->key == Events::Key::V and ctrl)
                paste(clipboard(NONE));
            else if (ke->key == Events::Key::Z and ctrl)
                undo();
            else if (ke->key == Events::Key::Z and ctrl and shift)
                redo();
            else if (ke->key == Events::Key::ENTER and nomod)
                newline();
            else
                return;

            e.accept();
        }
    }
};

} // namespace Textbox
