#include <karm-app/inputs.h>
#include <karm-logger/logger.h>

#include "edit.h"

namespace Karm::Text {

static bool _isWord(Rune r) {
    return isAsciiAlphaNum(r) or r == '_';
}

// MARK: Actions ---------------------------------------------------------------

Opt<Action> Action::fromEvent(App::Event &e) {
    if (auto te = e.is<App::TypeEvent>()) {
        return Action{TYPE, te->rune};
    } else if (
        auto ke = e.is<App::KeyboardEvent>();
        ke and ke->type == App::KeyboardEvent::PRESS
    ) {
        bool shift = !!(ke->mods & App::KeyMod::SHIFT);
        bool ctrl = !!(ke->mods & App::KeyMod::CTRL);
        bool alt = !!(ke->mods & App::KeyMod::ALT);
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

// MARK: Model -----------------------------------------------------------------

void Model::_do(Record &r) {
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

void Model::_undo(Record &r) {
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

void Model::_push(Op op, usize pos, Rune rune) {
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

void Model::_insertTo(usize pos, Rune rune) {
    _push(INSERT, pos, rune);
}

void Model::_moveTo(usize pos) {
    _push(MOVE, pos, 0);
}

void Model::_selectTo(usize pos) {
    _push(SELECT, pos, 0);
}

void Model::_deleteTo(usize pos) {
    _push(DELETE, pos, 0);
}

bool Model::_deleteSel() {
    if (not _cur.open())
        return false;

    _deleteTo(_cur.tail);
    _cur.head = min(_cur.head, _cur.tail);
    _cur.tail = _cur.head;

    return true;
}

void Model::_checkpoint() {
    _group++;
}

// MARK: Movements

usize Model::_prev(usize pos) const {
    if (pos == 0)
        return 0;

    return pos - 1;
}

usize Model::_next(usize pos) const {
    if (pos == _buf.len())
        return pos;

    return pos + 1;
}

usize Model::_up(usize pos) const {
    auto off = pos - _lineStart(pos);
    auto prevLine = _lineStart(_prevLine(pos));
    return min(prevLine + off, _lineEnd(prevLine));
}

usize Model::_down(usize pos) const {
    auto off = pos - _lineStart(pos);
    auto nextLine = _lineStart(_nextLine(pos));
    return min(nextLine + off, _lineEnd(nextLine));
}

usize Model::_prevWord(usize pos) const {
    auto startedFromWord = pos != 0 and _isWord(_buf[pos - 1]);

    if (not startedFromWord)
        while (pos != 0 and not _isWord(_buf[pos - 1]))
            pos--;

    while (pos != 0 and _isWord(_buf[pos - 1]))
        pos--;

    return pos;
}

usize Model::_nextWord(usize pos) const {
    auto startedFromWord = pos != _buf.len() and _isWord(_buf[pos]);

    if (not startedFromWord)
        while (pos != _buf.len() and not _isWord(_buf[pos]))
            pos++;

    while (pos != _buf.len() and _isWord(_buf[pos]))
        pos++;

    return pos;
}

usize Model::_lineStart(usize pos) const {
    while (pos != 0 and _buf[pos - 1] != '\n')
        pos--;

    return pos;
}

usize Model::_lineEnd(usize pos) const {
    while (pos != _buf.len() and _buf[pos] != '\n')
        pos++;

    return pos;
}

usize Model::_prevLine(usize pos) const {
    return _prev(_lineStart(pos));
}

usize Model::_nextLine(usize pos) const {
    return _next(_lineEnd(pos));
}

usize Model::_textStart(usize) const {
    return 0;
}

usize Model::_textEnd(usize) const {
    return _buf.len();
}

// MARK: Commands

void Model::insert(Rune rune) {
    _checkpoint();
    _deleteSel();
    _insertTo(_cur.head, rune);
    _moveTo(_next(_cur.head));
}

void Model::newline() {
    insert('\n');
}

void Model::backspace() {
    _checkpoint();
    if (not _deleteSel())
        _deleteTo(_prev(_cur.head));
}

void Model::delete_() {
    _checkpoint();
    if (not _deleteSel())
        _deleteTo(_next(_cur.head));
}

void Model::deletePrevWord() {
    _checkpoint();
    if (not _deleteSel())
        _deleteTo(_prevWord(_cur.head));
}

void Model::deleteNextWord() {
    _checkpoint();
    if (not _deleteSel())
        _deleteTo(_nextWord(_cur.head));
}

void Model::movePrev() {
    if (_cur.open())
        _moveTo(_cur.head);
    else
        _moveTo(_prev(_cur.head));
}

void Model::moveNext() {
    if (_cur.open())
        _moveTo(_cur.head);
    else
        _moveTo(_next(_cur.head));
}

void Model::moveUp() {
    _moveTo(_up(_cur.head));
}

void Model::moveDown() {
    _moveTo(_down(_cur.head));
}

void Model::movePrevWord() {
    _moveTo(_prevWord(_cur.head));
}

void Model::moveNextWord() {
    _moveTo(_nextWord(_cur.head));
}

void Model::moveLineStart() {
    _moveTo(_lineStart(_cur.head));
}

void Model::moveLineEnd() {
    _moveTo(_lineEnd(_cur.head));
}

void Model::moveStart() {
    _moveTo(_textStart(_cur.head));
}

void Model::moveEnd() {
    _moveTo(_textEnd(_cur.head));
}

void Model::selectPrev() {
    _selectTo(_prev(_cur.head));
}

void Model::selectNext() {
    _selectTo(_next(_cur.head));
}

void Model::selectUp() {
    _selectTo(_up(_cur.head));
}

void Model::selectDown() {
    _selectTo(_down(_cur.head));
}

void Model::selectPrevWord() {
    _selectTo(_prevWord(_cur.head));
}

void Model::selectNextWord() {
    _selectTo(_nextWord(_cur.head));
}

void Model::selectLineStart() {
    _selectTo(_lineStart(_cur.head));
}

void Model::selectLineEnd() {
    _selectTo(_lineEnd(_cur.head));
}

void Model::selectStart() {
    _selectTo(_textStart(_cur.head));
}

void Model::selectEnd() {
    _selectTo(_textEnd(_cur.head));
}

void Model::selectAll() {
    _moveTo(_textStart(_cur.head));
    _selectTo(_textEnd(_cur.head));
}

String Model::copy() {
    StringBuilder sb;
    sb.append(sub(_buf, _cur.head, _cur.tail));
    return sb.take();
}

String Model::cut() {
    _checkpoint();
    auto str = copy();
    _deleteSel();
    return str;
}

void Model::paste(Str text) {
    _checkpoint();
    _deleteSel();
    for (auto rune : iterRunes(text))
        _insertTo(_cur.head, rune);
}

void Model::undo() {
    if (_index == 0)
        return;

    auto group = _records[_index - 1].group;
    while (_index != 0 and _records[_index - 1].group == group) {
        _undo(_records[_index - 1]);
        _index--;
    }
}

void Model::redo() {
    if (_index == _records.len())
        return;

    auto group = _records[_index].group;
    while (_index != _records.len() and _records[_index].group == group) {
        _do(_records[_index]);
        _index++;
    }
}

void Model::flush() {
    _records.clear();
}

void Model::reduce(Action const &a) {
    switch (a.op) {
    case Action::TYPE:
        insert(a.rune);
        break;

    case Action::NEWLINE:
        newline();
        break;

    case Action::BACKSPACE:
        backspace();
        break;

    case Action::DELETE:
        delete_();
        break;

    case Action::DELETE_PREV_WORD:
        deletePrevWord();
        break;

    case Action::DELETE_NEXT_WORD:
        deleteNextWord();
        break;

    case Action::MOVE_PREV:
        movePrev();
        break;

    case Action::MOVE_NEXT:
        moveNext();
        break;

    case Action::MOVE_UP:
        moveUp();
        break;

    case Action::MOVE_DOWN:
        moveDown();
        break;

    case Action::MOVE_PREV_WORD:
        movePrevWord();
        break;

    case Action::MOVE_NEXT_WORD:
        moveNextWord();
        break;

    case Action::MOVE_LINE_START:
        moveLineStart();
        break;

    case Action::MOVE_LINE_END:
        moveLineEnd();
        break;

    case Action::MOVE_START:
        moveStart();
        break;

    case Action::MOVE_END:
        moveEnd();
        break;

    case Action::SELECT_PREV:
        selectPrev();
        break;

    case Action::SELECT_NEXT:
        selectNext();
        break;

    case Action::SELECT_UP:
        selectUp();
        break;

    case Action::SELECT_DOWN:
        selectDown();
        break;

    case Action::SELECT_PREV_WORD:
        selectPrevWord();
        break;

    case Action::SELECT_NEXT_WORD:
        selectNextWord();
        break;

    case Action::SELECT_LINE_START:
        selectLineStart();
        break;

    case Action::SELECT_LINE_END:
        selectLineEnd();
        break;

    case Action::SELECT_START:
        selectStart();
        break;

    case Action::SELECT_END:
        selectEnd();
        break;

    case Action::SELECT_ALL:
        selectAll();
        break;

    case Action::UNDO:
        undo();
        break;

    case Action::REDO:
        redo();
        break;

    default:
        break;
    }
}

} // namespace Karm::Text
