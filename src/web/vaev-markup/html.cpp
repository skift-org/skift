#include <karm-base/array.h>
#include <karm-base/cons.h>

#include "html.h"

namespace Vaev::Markup {

static constexpr bool DEBUG_HTML_PARSER = false;

// MARK: Lexer -----------------------------------------------------------------

struct Entity {
    Str name;
    Rune const *runes;

    operator bool() {
        return runes != nullptr;
    }
};

[[maybe_unused]] static Array const ENTITIES = {
#define ENTITY(NAME, ...) \
    Entity{#NAME, (Rune[]){__VA_ARGS__ __VA_OPT__(, ) 0}},
#include "defs/ns-html-entities.inc"
#undef ENTITY
};

void HtmlLexer::_raise(Str msg) {
    logError("{}: {}", _state, msg);
}

void HtmlLexer::consume(Rune rune, bool isEof) {
    logDebugIf(DEBUG_HTML_PARSER, "Lexing '{#c}' {#x} in {}", rune, rune, _state);

    switch (_state) {

    case State::DATA: {
        // 13.2.5.1 MARK: Data state
        // Consume the next input character:

        // U+0026 AMPERSAND (&)
        // Set the return state to the data state. Switch to the character
        // reference state.
        if (rune == '&') {
            _returnState = State::DATA;
            _switchTo(State::CHARACTER_REFERENCE);
        }

        // U+003C LESS-THAN SIGN (<)
        // Switch to the tag open state.
        else if (rune == '<') {
            _switchTo(State::TAG_OPEN);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Emit the
        // current input character as a character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _emit(rune);
        }

        // EOF
        // Emit an end-of-file token.
        else if (isEof) {
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Emit the current input character as a character token.
        else {
            _emit(rune);
        }

        break;
    }

    case State::RCDATA: {
        // 13.2.5.2 MARK: RCDATA state
        // Consume the next input character:

        // U+0026 AMPERSAND (&)
        // Set the return state to the RCDATA state. Switch to the character
        // reference state.
        if (rune == '&') {
            _returnState = State::RCDATA;
            _switchTo(State::CHARACTER_REFERENCE);
        }

        // U+003C LESS-THAN SIGN (<)
        // Switch to the RCDATA less-than sign state.
        else if (rune == '<')
            _switchTo(State::RCDATA_LESS_THAN_SIGN);

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Emit a U+FFFD
        // REPLACEMENT CHARACTER character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _emit(0xFFFD);
        }

        // EOF
        // Emit an end-of-file token.
        else if (isEof) {
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Emit the current input character as a character token.
        else {
            _emit(rune);
        }

        break;
    }

    case State::RAWTEXT: {
        // 13.2.5.3 MARK: RAWTEXT state
        // Consume the next input character:

        // U+003C LESS-THAN SIGN (<)
        // Switch to the RAWTEXT less-than sign state.
        if (rune == '<') {
            _switchTo(State::RAWTEXT_LESS_THAN_SIGN);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Emit a U+FFFD
        // REPLACEMENT CHARACTER character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _emit(0xFFFD);
        }

        // EOF
        // Emit an end-of-file token.
        else if (isEof) {
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Emit the current input character as a character token.
        else {
            _emit(rune);
        }

        break;
    }

    case State::SCRIPT_DATA: {
        // 13.2.5.4 MARK: Script data state
        // Consume the next input character:

        // U+003C LESS-THAN SIGN (<)
        // Switch to the script data less-than sign state.
        if (rune == '<') {
            _switchTo(State::SCRIPT_DATA_LESS_THAN_SIGN);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Emit a U+FFFD
        // REPLACEMENT CHARACTER character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _emit(0xFFFD);
        }

        // EOF
        // Emit an end-of-file token.
        else if (isEof) {
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Emit the current input character as a character token.
        else {
            _emit(rune);
        }

        break;
    }

    case State::PLAINTEXT: {
        // 13.2.5.5 MARK: PLAINTEXT state
        // Consume the next input character:

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Emit a U+FFFD
        // REPLACEMENT CHARACTER character token.
        if (rune == 0) {
            _raise("unexpected-null-character");
            _emit(0xFFFD);
        }

        // EOF
        // Emit an end-of-file token.
        else if (isEof) {
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Emit the current input character as a character token.
        else {
            _emit(rune);
        }

        break;
    }

    case State::TAG_OPEN: {
        // 13.2.5.6 MARK: Tag open state
        // Consume the next input character:

        // U+0021 EXCLAMATION MARK (!)
        // Switch to the markup declaration open state.
        if (rune == '!') {
            _switchTo(State::MARKUP_DECLARATION_OPEN);
        }

        // U+002F SOLIDUS (/)
        // Switch to the end tag open state.
        else if (rune == '/') {
            _switchTo(State::END_TAG_OPEN);
        }

        // ASCII alpha
        // Create a new start tag token, set its tag name to the empty
        // string. Reconsume in the tag name state.
        else if (isAsciiAlpha(rune)) {
            _begin(HtmlToken::START_TAG);
            _reconsumeIn(State::TAG_NAME, rune);
        }

        // U+003F QUESTION MARK (?)
        // This is an unexpected-question-mark-instead-of-tag-name parse
        // error. Create a comment token whose data is the empty string.
        // Reconsume in the bogus comment state.
        else if (rune == '?') {
            _raise("unexpected-question-mark-instead-of-tag-name");
            _begin(HtmlToken::COMMENT);
            _reconsumeIn(State::BOGUS_COMMENT, rune);
        }

        // EOF
        // This is an eof-before-tag-name parse error. Emit a U+003C
        // LESS-THAN SIGN character token and an end-of-file token.
        else if (isEof) {
            _raise("eof-before-tag-name");
            _emit('<');
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is an invalid-first-character-of-tag-name parse error. Emit
        // a U+003C LESS-THAN SIGN character token. Reconsume in the data
        // state.
        else {
            _raise("invalid-first-character-of-tag-name");
            _emit('<');
            _reconsumeIn(State::DATA, rune);
        }

        break;
    }

    case State::END_TAG_OPEN: {
        // 13.2.5.7 MARK: End tag open state
        // Consume the next input character:

        // ASCII alpha
        // Create a new end tag token, set its tag name to the empty string.
        // Reconsume in the tag name state.
        if (isAsciiAlpha(rune)) {
            _begin(HtmlToken::END_TAG);
            _reconsumeIn(State::TAG_NAME, rune);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is a missing-end-tag-name parse error. Switch to the data
        // state.
        else if (rune == '>') {
            _raise("missing-end-tag-name");
            _switchTo(State::DATA);
        }

        // EOF
        // This is an eof-before-tag-name parse error. Emit a U+003C
        // LESS-THAN SIGN character token, a U+002F SOLIDUS character token
        // and an end-of-file token.
        else if (isEof) {
            _raise("eof-before-tag-name");
            _emit('<');
            _emit('/');
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is an invalid-first-character-of-tag-name parse error.
        // Create a comment token whose data is the empty string. Reconsume
        // in the bogus comment state.
        else {
            _raise("invalid-first-character-of-tag-name");
            _begin(HtmlToken::COMMENT);
            _reconsumeIn(State::BOGUS_COMMENT, rune);
        }

        break;
    }

    case State::TAG_NAME: {
        // 13.2.5.8 MARK: Tag name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Switch to the before attribute name state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            _ensure().name = _builder.take();
            _switchTo(State::BEFORE_ATTRIBUTE_NAME);
        }

        // U+002F SOLIDUS (/)
        // Switch to the self-closing start tag state.
        else if (rune == '/') {
            _ensure().name = _builder.take();
            _switchTo(State::SELF_CLOSING_START_TAG);
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current tag token.
        else if (rune == '>') {
            _ensure().name = _builder.take();
            _switchTo(State::DATA);
            _emit();
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character (add
        // 0x0020 to the character's code point) to the current tag token's
        // tag name.
        else if (isAsciiUpper(rune)) {
            _builder.append(toAsciiLower(rune));
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current tag token's tag
        // name.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // EOF
        // This is an eof-in-tag parse error. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-tag");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current tag token's tag
        // name.
        else {
            _builder.append(rune);
        }

        break;
    }

    case State::RCDATA_LESS_THAN_SIGN: {
        // 13.2.5.9 MARK: RCDATA less-than sign state
        // Consume the next input character:

        // U+002F SOLIDUS (/)
        // Set the temporary buffer to the empty string. Switch to the
        // RCDATA end tag open state.
        if (rune == '/') {
            _temp.clear();
            _switchTo(State::RCDATA_END_TAG_OPEN);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token. Reconsume in the
        // RCDATA state.
        else {
            _emit('<');
            _reconsumeIn(State::RCDATA, rune);
        }

        break;
    }

    case State::RCDATA_END_TAG_OPEN: {
        // 13.2.5.10 MARK: RCDATA end tag open state
        // Consume the next input character:

        // ASCII alpha
        // Create a new end tag token, set its tag name to the empty string.
        // Reconsume in the RCDATA end tag name state.
        if (isAsciiAlpha(rune)) {
            _begin(HtmlToken::END_TAG);
            _reconsumeIn(State::RCDATA_END_TAG_NAME, rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token and a U+002F SOLIDUS
        // character token. Reconsume in the RCDATA state.
        else {
            _emit('<');
            _emit('/');
            _reconsumeIn(State::RCDATA, rune);
        }

        break;
    }

    case State::RCDATA_END_TAG_NAME: {
        // 13.2.5.11 MARK: RCDATA end tag name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // If the current end tag token is an appropriate end tag token,
        // then switch to the before attribute name state. Otherwise,
        // treat it as per the "anything else" entry below.
        if ((rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') and
            _isAppropriateEndTagToken()) {
            _ensure().name = _builder.take();
            _switchTo(State::BEFORE_ATTRIBUTE_NAME);
        }

        // U+002F SOLIDUS (/)
        // If the current end tag token is an appropriate end tag token,
        // then switch to the self-closing start tag state. Otherwise,
        // treat it as per the "anything else" entry below.
        else if (rune == '/' and _isAppropriateEndTagToken()) {
            _ensure().name = _builder.take();
            _switchTo(State::SELF_CLOSING_START_TAG);
        }

        // U+003E GREATER-THAN SIGN (>)
        // If the current end tag token is an appropriate end tag token,
        // then switch to the data state and emit the current tag token.
        // Otherwise, treat it as per the "anything else" entry below.
        else if (rune == '>' and _isAppropriateEndTagToken()) {
            _ensure().name = _builder.take();
            _switchTo(State::DATA);
            _emit();
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character
        // (add 0x0020 to the character's code point) to the current tag
        // token's tag name. Append the current input character to the
        // temporary buffer.
        else if (isAsciiUpper(rune)) {
            _builder.append(toAsciiLower(rune));
            _temp.append(rune);
        }

        // ASCII lower alpha
        // Append the current input character to the current tag token's
        // tag name. Append the current input character to the temporary
        // buffer.
        else if (isAsciiLower(rune)) {
            _builder.append(rune);
            _temp.append(rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token, a U+002F
        // SOLIDUS character token, and a character token for each of
        // the characters in the temporary buffer (in the order they
        // were added to the buffer). Reconsume in the RCDATA state.
        else {
            _emit('<');
            _emit('/');
            for (Rune rune : iterRunes(_temp.str()))
                _emit(rune);

            _reconsumeIn(State::RCDATA, rune);
        }

        break;
    }
    case State::RAWTEXT_LESS_THAN_SIGN: {
        // 13.2.5.12 MARK: RAWTEXT less-than sign state
        // Consume the next input character:

        // U+002F SOLIDUS (/)
        // Set the temporary buffer to the empty string. Switch to the
        // RAWTEXT end tag open state.
        if (rune == '/') {
            _temp.clear();
            _switchTo(State::RAWTEXT_END_TAG_OPEN);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token. Reconsume in the
        // RAWTEXT state.
        else {
            _emit('<');
            _reconsumeIn(State::RAWTEXT, rune);
        }

        break;
    }

    case State::RAWTEXT_END_TAG_OPEN: {
        // 13.2.5.13 MARK: RAWTEXT end tag open state
        // Consume the next input character:

        // ASCII alpha
        // Create a new end tag token, set its tag name to the empty string.
        // Reconsume in the RAWTEXT end tag name state.
        if (isAsciiAlpha(rune)) {
            _begin(HtmlToken::END_TAG);
            _reconsumeIn(State::RAWTEXT_END_TAG_NAME, rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token and a U+002F SOLIDUS
        // character token. Reconsume in the RAWTEXT state.
        else {
            _emit('<');
            _emit('/');
            _reconsumeIn(State::RAWTEXT, rune);
        }

        break;
    }

    case State::RAWTEXT_END_TAG_NAME: {
        // 13.2.5.14 MARK: RAWTEXT end tag name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // If the current end tag token is an appropriate end tag token,
        // then switch to the before attribute name state. Otherwise,
        // treat it as per the "anything else" entry below.
        if ((rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') and
            _isAppropriateEndTagToken()) {
            _switchTo(State::BEFORE_ATTRIBUTE_NAME);
        }

        // U+002F SOLIDUS (/)
        // If the current end tag token is an appropriate end tag token,
        // then switch to the self-closing start tag state. Otherwise,
        // treat it as per the "anything else" entry below.
        else if (rune == '/' and _isAppropriateEndTagToken()) {
            _switchTo(State::SELF_CLOSING_START_TAG);
        }

        // U+003E GREATER-THAN SIGN (>)
        // If the current end tag token is an appropriate end tag token,
        // then switch to the data state and emit the current tag token.
        // Otherwise, treat it as per the "anything else" entry below.
        else if (rune == '>' and _isAppropriateEndTagToken()) {
            _switchTo(State::DATA);
            _emit();
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character (add
        // 0x0020 to the character's code point) to the current tag token's
        // tag name. Append the current input character to the temporary
        // buffer.
        else if (isAsciiUpper(rune)) {
            _builder.append(toAsciiLower(rune));
            _temp.append(rune);
        }

        // ASCII lower alpha
        // Append the current input character to the current tag token's tag
        // name. Append the current input character to the temporary buffer.
        else if (isAsciiLower(rune)) {
            _builder.append(rune);
            _temp.append(rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token, a U+002F SOLIDUS
        // character token, and a character token for each of the characters
        // in the temporary buffer (in the order they were added to the
        // buffer). Reconsume in the RAWTEXT state.
        else {
            _emit('<');
            _emit('/');
            for (Rune rune : iterRunes(_temp.str()))
                _emit(rune);
            _reconsumeIn(State::RAWTEXT, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_LESS_THAN_SIGN: {
        // 13.2.5.15 MARK: Script data less-than sign state
        // Consume the next input character:

        // U+002F SOLIDUS (/)
        // Set the temporary buffer to the empty string. Switch to the
        // script data end tag open state.
        if (rune == '/') {
            _temp.clear();
            _switchTo(State::SCRIPT_DATA_END_TAG_OPEN);
        }

        // U+0021 EXCLAMATION MARK (!)
        // Switch to the script data escape start state. Emit a U+003C
        // LESS-THAN SIGN character token and a U+0021 EXCLAMATION MARK
        // character token.
        if (rune == '!') {
            _switchTo(State::SCRIPT_DATA_ESCAPE_START);
            _emit('<');
            _emit('!');
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token. Reconsume in the
        // script data state.
        else {
            _emit('<');
            _reconsumeIn(State::SCRIPT_DATA, rune);
        }
        break;
    }

    case State::SCRIPT_DATA_END_TAG_OPEN: {
        // 13.2.5.16 MARK: Script data end tag open state
        // Consume the next input character:

        // ASCII alpha
        // Create a new end tag token, set its tag name to the empty string.
        // Reconsume in the script data end tag name state.
        if (isAsciiAlpha(rune)) {
            _begin(HtmlToken::END_TAG);
            _reconsumeIn(State::SCRIPT_DATA_END_TAG_NAME, rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token and a U+002F SOLIDUS
        // character token. Reconsume in the script data state.
        else {
            _emit('<');
            _emit('/');
            _reconsumeIn(State::SCRIPT_DATA, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_END_TAG_NAME: {
        // 13.2.5.17 MARK: Script data end tag name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // If the current end tag token is an appropriate end tag token,
        // then switch to the before attribute name state. Otherwise,
        // treat it as per the "anything else" entry below.

        if ((rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') and
            _isAppropriateEndTagToken()) {
            _switchTo(State::BEFORE_ATTRIBUTE_NAME);
        }

        // U+002F SOLIDUS (/)
        // If the current end tag token is an appropriate end tag token,
        // then switch to the self-closing start tag state. Otherwise,
        // treat it as per the "anything else" entry below.
        else if (rune == '/' and _isAppropriateEndTagToken()) {
            _switchTo(State::SELF_CLOSING_START_TAG);
        }

        // U+003E GREATER-THAN SIGN (>)
        // If the current end tag token is an appropriate end tag token,
        // then switch to the data state and emit the current tag token.
        // Otherwise, treat it as per the "anything else" entry below.
        else if (rune == '>' and _isAppropriateEndTagToken()) {
            _switchTo(State::DATA);
            _emit();
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character
        // (add 0x0020 to the character's code point) to the current tag
        // token's tag name. Append the current input character to the
        // temporary buffer.
        else if (isAsciiUpper(rune)) {
            _builder.append(toAsciiLower(rune));
            _temp.append(rune);
        }

        // ASCII lower alpha
        // Append the current input character to the current tag token's
        // tag name. Append the current input character to the temporary
        // buffer.
        else if (isAsciiLower(rune)) {
            _builder.append(rune);
            _temp.append(rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token, a U+002F
        // SOLIDUS character token, and a character token for each of
        // the characters in the temporary buffer (in the order they
        // were added to the buffer). Reconsume in the script data
        // state.
        else {
            _emit('<');
            _emit('/');
            for (Rune rune : iterRunes(_temp.str()))
                _emit(rune);
            _reconsumeIn(State::SCRIPT_DATA, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_ESCAPE_START: {
        // 13.2.5.18 MARK: Script data escape start state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the script data escape start dash state. Emit a U+002D
        // HYPHEN-MINUS character token.
        if (rune == '-') {
            _switchTo(State::SCRIPT_DATA_ESCAPE_START_DASH);
            _emit('-');
        }

        // Anything else
        // Reconsume in the script data state.
        else {
            _reconsumeIn(State::SCRIPT_DATA, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_ESCAPE_START_DASH: {
        // 13.2.5.19 MARK: Script data escape start dash state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the script data escaped dash dash state. Emit a U+002D
        // HYPHEN-MINUS character token.
        if (rune == '-') {
            _switchTo(State::SCRIPT_DATA_ESCAPED_DASH_DASH);
            _emit('-');
        }

        // Anything else
        // Reconsume in the script data state.
        else {
            _reconsumeIn(State::SCRIPT_DATA, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_ESCAPED: {
        // 13.2.5.20 MARK: Script data escaped state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the script data escaped dash state. Emit a U+002D
        // HYPHEN-MINUS character token.
        if (rune == '-') {
            _switchTo(State::SCRIPT_DATA_ESCAPED_DASH);
            _emit('-');
        }

        // U+003C LESS-THAN SIGN (<)
        // Switch to the script data escaped less-than sign state.
        else if (rune == '<') {
            _switchTo(State::SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Emit a U+FFFD
        // REPLACEMENT CHARACTER character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _emit(0xFFFD);
        }

        // EOF
        // This is an eof-in-script-html-comment-like-text parse error. Emit
        // an end-of-file token.
        else if (isEof) {
            _raise("eof-in-script-html-comment-like-text");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Emit the current input character as a character token.
        else {
            _emit(rune);
        }

        break;
    }

    case State::SCRIPT_DATA_ESCAPED_DASH: {
        // 13.2.5.21 MARK: Script data escaped dash state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the script data escaped dash dash state. Emit a U+002D
        // HYPHEN-MINUS character token.
        if (rune == '-') {
            _switchTo(State::SCRIPT_DATA_ESCAPED_DASH_DASH);
            _emit('-');
        }

        // U+003C LESS-THAN SIGN (<)
        // Switch to the script data escaped less-than sign state.
        if (rune == '<') {
            _switchTo(State::SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Switch to the
        // script data escaped state. Emit a U+FFFD REPLACEMENT CHARACTER
        // character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _switchTo(State::SCRIPT_DATA_ESCAPED);
            _emit(0xFFFD);
        }

        // EOF
        else if (isEof) {
            _raise("eof-in-script-html-comment-like-text");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Switch to the script data state. Emit a U+003E GREATER-THAN SIGN
        // character token.
        else if (rune == '>') {
            _switchTo(State::SCRIPT_DATA);
            _emit('>');
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Switch to the
        // script data escaped state. Emit a U+FFFD REPLACEMENT CHARACTER
        // character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _switchTo(State::SCRIPT_DATA_ESCAPED);
            _emit(0xFFFD);
        }

        // EOF
        // This is an eof-in-script-html-comment-like-text parse error. Emit
        // an end-of-file token.
        else if (isEof) {
            _raise("eof-in-script-html-comment-like-text");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Switch to the script data escaped state. Emit the current input
        // character as a character token.
        else {
            _switchTo(State::SCRIPT_DATA_ESCAPED);
            _emit(rune);
        }

        break;
    }

    case State::SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN: {
        // 13.2.5.23 MARK: Script data escaped less-than sign state
        // Consume the next input character:

        // U+002F SOLIDUS (/)
        // Set the temporary buffer to the empty string. Switch to the
        // script data escaped end tag open state.
        if (rune == '/') {
            _temp.clear();
            _switchTo(State::SCRIPT_DATA_ESCAPED_END_TAG_OPEN);
        }

        // ASCII alpha
        // Set the temporary buffer to the empty string. Emit a U+003C
        // LESS-THAN SIGN character token. Reconsume in the script data
        // double escape start state.
        else if (isAsciiAlpha(rune)) {
            _temp.clear();
            _emit('<');
            _reconsumeIn(State::SCRIPT_DATA_DOUBLE_ESCAPE_START, rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token. Reconsume in the
        // script data escaped state.
        else {
            _emit('<');
            _reconsumeIn(State::SCRIPT_DATA_ESCAPED, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_ESCAPED_END_TAG_OPEN: {
        // 13.2.5.24 MARK: Script data escaped end tag open state
        // Consume the next input character:

        // ASCII alpha
        // Create a new end tag token, set its tag name to the empty string.
        // Reconsume in the script data escaped end tag name state.
        if (isAsciiAlpha(rune)) {
            _begin(HtmlToken::END_TAG);
            _reconsumeIn(State::SCRIPT_DATA_ESCAPED_END_TAG_NAME, rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token and a U+002F SOLIDUS
        // character token. Reconsume in the script data escaped state.
        else {
            _emit('<');
            _emit('/');
            _reconsumeIn(State::SCRIPT_DATA_ESCAPED, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_ESCAPED_END_TAG_NAME: {
        // 13.2.5.25 MARK: Script data escaped end tag name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // If the current end tag token is an appropriate end tag token,
        // then switch to the before attribute name state. Otherwise, treat
        // it as per the "anything else" entry below.
        if ((rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') and
            _isAppropriateEndTagToken()) {
            _switchTo(State::BEFORE_ATTRIBUTE_NAME);
        }

        // U+002F SOLIDUS (/)
        // If the current end tag token is an appropriate end tag token,
        // then switch to the self-closing start tag state. Otherwise, treat
        // it as per the "anything else" entry below.
        else if (rune == '/' and _isAppropriateEndTagToken()) {
            _switchTo(State::SELF_CLOSING_START_TAG);
        }

        // U+003E GREATER-THAN SIGN (>)
        // If the current end tag token is an appropriate end tag token,
        // then switch to the data state and emit the current tag token.
        // Otherwise, treat it as per the "anything else" entry below.
        else if (rune == '>' and _isAppropriateEndTagToken()) {
            _switchTo(State::DATA);
            _emit();
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character (add
        // 0x0020 to the character's code point) to the current tag token's
        // tag name. Append the current input character to the temporary
        // buffer.
        else if (isAsciiUpper(rune)) {
            _builder.append(toAsciiLower(rune));
            _temp.append(rune);
        }

        // ASCII lower alpha
        // Append the current input character to the current tag token's tag
        // name. Append the current input character to the temporary buffer.
        else if (isAsciiLower(rune)) {
            _builder.append(rune);
            _temp.append(rune);
        }

        // Anything else
        // Emit a U+003C LESS-THAN SIGN character token, a U+002F SOLIDUS
        // character token, and a character token for each of the characters
        // in the temporary buffer (in the order they were added to the
        // buffer). Reconsume in the script data escaped state.
        else {
            _emit('<');
            _emit('/');
            for (Rune rune : iterRunes(_temp.str()))
                _emit(rune);
            _reconsumeIn(State::SCRIPT_DATA_ESCAPED, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_DOUBLE_ESCAPE_START: {
        // 13.2.5.26 MARK: Script data double escape start state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // U+002F SOLIDUS (/)
        // U+003E GREATER-THAN SIGN (>)
        // If the temporary buffer is the string "script", then switch to
        // the script data double escaped state. Otherwise, switch to the
        // script data escaped state. Emit the current input character as a
        // character token.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ' or
            rune == '/' or rune == '>') {
            if (_temp.str() == "script")
                _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED);
            else
                _switchTo(State::SCRIPT_DATA_ESCAPED);

            _emit(rune);
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character (add
        // 0x0020 to the character's code point) to the temporary buffer.
        // Emit the current input character as a character token.
        else if (isAsciiUpper(rune)) {
            _temp.append(toAsciiLower(rune));
            _emit(rune);
        }

        // ASCII lower alpha
        // Append the current input character to the temporary buffer. Emit
        // the current input character as a character token.
        else if (isAsciiLower(rune)) {
            _temp.append(rune);
            _emit(rune);
        }

        // Anything else
        // Reconsume in the script data escaped state.
        else {
            _reconsumeIn(State::SCRIPT_DATA_ESCAPED, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_DOUBLE_ESCAPED: {
        // 13.2.5.27 MARK: Script data double escaped state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the script data double escaped dash state. Emit a
        // U+002D HYPHEN-MINUS character token.
        if (rune == '-') {
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED_DASH);
            _emit('-');
        }

        // U+003C LESS-THAN SIGN (<)
        // Switch to the script data double escaped less-than sign state.
        // Emit a U+003C LESS-THAN SIGN character token.
        else if (rune == '<') {
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN);
            _emit('<');
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Emit a U+FFFD
        // REPLACEMENT CHARACTER character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _emit(0xFFFD);
        }

        // EOF
        // This is an eof-in-script-html-comment-like-text parse error. Emit
        // an end-of-file token.
        else if (isEof) {
            _raise("eof-in-script-html-comment-like-text");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Emit the current input character as a character token.
        else {
            _emit(rune);
        }

        break;
    }

    case State::SCRIPT_DATA_DOUBLE_ESCAPED_DASH: {
        // 13.2.5.28 MARK: Script data double escaped dash state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the script data double escaped dash dash state. Emit a
        // U+002D HYPHEN-MINUS character token.
        if (rune == '-') {
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH);
            _emit('-');
        }

        // U+003C LESS-THAN SIGN (<)
        // Switch to the script data double escaped less-than sign state.
        // Emit a U+003C LESS-THAN SIGN character token.
        else if (rune == '<') {
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN);
            _emit('<');
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Switch to the
        // script data double escaped state. Emit a U+FFFD REPLACEMENT
        // CHARACTER character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED);
            _emit(0xFFFD);
        }

        // EOF
        // This is an eof-in-script-html-comment-like-text parse error. Emit
        // an end-of-file token.
        else if (isEof) {
            _raise("eof-in-script-html-comment-like-text");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Switch to the script data double escaped state. Emit the current
        // input character as a character token.
        else {
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED);
            _emit(rune);
        }

        break;
    }

    case State::SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH: {
        // 13.2.5.29 MARK: Script data double escaped dash dash state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Emit a U+002D HYPHEN-MINUS character token.
        if (rune == '-') {
            _emit('-');
        }

        // U+003C LESS-THAN SIGN (<)
        // Switch to the script data double escaped less-than sign state.
        // Emit a U+003C LESS-THAN SIGN character token.
        else if (rune == '<') {
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN);
            _emit('<');
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the script data state. Emit a U+003E GREATER-THAN SIGN
        // character token.
        else if (rune == '>') {
            _switchTo(State::SCRIPT_DATA);
            _emit('>');
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Switch to the
        // script data double escaped state. Emit a U+FFFD REPLACEMENT
        // CHARACTER character token.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED);
            _emit(0xFFFD);
        }

        // EOF
        // This is an eof-in-script-html-comment-like-text parse error. Emit
        // an end-of-file token.
        else if (isEof) {
            _raise("eof-in-script-html-comment-like-text");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Switch to the script data double escaped state. Emit the current
        // input character as a character token.
        else {
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED);
            _emit(rune);
        }

        break;
    }

    case State::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN: {
        // 13.2.5.30 MARK: Script data double escaped less-than sign state
        // Consume the next input character:

        // U+002F SOLIDUS (/)
        // Set the temporary buffer to the empty string. Switch to the
        // script data double escape end state. Emit a U+002F SOLIDUS
        // character token.
        if (rune == '/') {
            _temp.clear();
            _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPE_END);
            _emit('/');
        }

        // Anything else
        // Reconsume in the script data double escaped state.
        else {
            _reconsumeIn(State::SCRIPT_DATA_DOUBLE_ESCAPED, rune);
        }

        break;
    }

    case State::SCRIPT_DATA_DOUBLE_ESCAPE_END: {
        // 13.2.5.31 MARK: Script data double escape end state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // U+002F SOLIDUS (/)
        // U+003E GREATER-THAN SIGN (>)
        // If the temporary buffer is the string "script", then switch to
        // the script data escaped state. Otherwise, switch to the script
        // data double escaped state. Emit the current input character as a
        // character token.

        if ((rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ' or
             rune == '/' or rune == '>')) {
            if (_temp.str() == "script") {
                _switchTo(State::SCRIPT_DATA_ESCAPED);
            } else {
                _switchTo(State::SCRIPT_DATA_DOUBLE_ESCAPED);
            }
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character (add
        // 0x0020 to the character's code point) to the temporary buffer.
        // Emit the current input character as a character token.
        else if (isAsciiUpper(rune)) {
            _temp.append(toAsciiLower(rune));
            _emit(rune);
        }

        // ASCII lower alpha
        // Append the current input character to the temporary buffer. Emit
        // the current input character as a character token.
        else if (isAsciiLower(rune)) {
            _temp.append(rune);
            _emit(rune);
        }

        // Anything else
        // Reconsume in the script data double escaped state.
        else {
            _reconsumeIn(State::SCRIPT_DATA_DOUBLE_ESCAPED, rune);
        }
        break;
    }

    case State::BEFORE_ATTRIBUTE_NAME: {
        // 13.2.5.32 MARK: Before attribute name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // U+002F SOLIDUS (/)
        // U+003E GREATER-THAN SIGN (>)
        // EOF
        // Reconsume in the after attribute name state.
        else if (rune == '/' or rune == '>' or isEof) {
            _reconsumeIn(State::AFTER_ATTRIBUTE_NAME, rune);
        }

        // U+003D EQUALS SIGN (=)
        // This is an unexpected-equals-sign-before-attribute-name parse
        // error. Start a new attribute in the current tag token. Set that
        // attribute's name to the current input character, and its value to
        // the empty string. Switch to the attribute name state.
        else if (rune == '=') {
            _raise("unexpected-equals-sign-before-attribute-name");
            _beginAttribute();
            _builder.append(rune);
            _switchTo(State::ATTRIBUTE_NAME);
        }

        // Anything else
        // Start a new attribute in the current tag token. Set that
        // attribute name and value to the empty string. Reconsume in the
        // attribute name state.
        else {
            _beginAttribute();
            _reconsumeIn(State::ATTRIBUTE_NAME, rune);
        }

        break;
    }

    case State::ATTRIBUTE_NAME: {
        // 13.2.5.33 MARK: Attribute name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // U+002F SOLIDUS (/)
        // U+003E GREATER-THAN SIGN (>)
        // EOF
        // Reconsume in the after attribute name state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ' or
            rune == '/' or rune == '>' or isEof) {
            _lastAttr().name = _builder.take();
            _reconsumeIn(State::AFTER_ATTRIBUTE_NAME, rune);
        }

        // U+003D EQUALS SIGN (=)
        // Switch to the before attribute value state.
        else if (rune == '=') {
            _lastAttr().name = _builder.take();
            _switchTo(State::BEFORE_ATTRIBUTE_VALUE);
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character (add
        // 0x0020 to the character's code point) to the current attribute's
        // name.
        else if (isAsciiUpper(rune)) {
            _builder.append(toAsciiLower(rune));
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current attribute's name.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // U+0022 QUOTATION MARK (")
        // U+0027 APOSTROPHE (')
        // U+003C LESS-THAN SIGN (<)
        // This is an unexpected-character-in-attribute-name parse error.
        // Treat it as per the "anything else" entry below.
        else if (rune == '"' or rune == '\'' or rune == '<') {
            _raise("unexpected-character-in-attribute-name");
            _builder.append(rune);
        }

        // Anything else
        // Append the current input character to the current attribute's
        // name. When the user agent leaves the attribute name state (and
        // before emitting the tag token, if appropriate), the complete
        // attribute's name must be compared to the other attributes on the
        // same token; if there is already an attribute on the token with
        // the exact same name, then this is a duplicate-attribute parse
        // error and the new attribute must be removed from the token.
        // If an attribute is so removed from a token, it, and the value
        // that gets associated with it, if any, are never subsequently used
        // by the parser, and are therefore effectively discarded. Removing
        // the attribute in this way does not change its status as the
        // "current attribute" for the purposes of the lexer, however.
        else if (isAsciiLower(rune)) {
            _builder.append(rune);
        }

        break;
    }

    case State::AFTER_ATTRIBUTE_NAME: {
        // 13.2.5.34 MARK: After attribute name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // U+002F SOLIDUS (/)
        // Switch to the self-closing start tag state.
        else if (rune == '/') {
            _switchTo(State::SELF_CLOSING_START_TAG);
        }

        // U+003D EQUALS SIGN (=)
        // Switch to the before attribute value state.
        else if (rune == '=') {
            _switchTo(State::BEFORE_ATTRIBUTE_VALUE);
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current tag token.
        else if (rune == '>') {
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-tag parse error. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-tag");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Start a new attribute in the current tag token. Set that
        // attribute name and value to the empty string. Reconsume in the
        // attribute name state.
        else {
            _beginAttribute();
            _reconsumeIn(State::ATTRIBUTE_NAME, rune);
        }

        break;
    }

    case State::BEFORE_ATTRIBUTE_VALUE: {
        // 13.2.5.35 MARK: Before attribute value state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // U+0022 QUOTATION MARK (")
        // Switch to the attribute value (double-quoted) state.
        else if (rune == '"') {
            _switchTo(State::ATTRIBUTE_VALUE_DOUBLE_QUOTED);
        }

        // U+0027 APOSTROPHE (')
        // Switch to the attribute value (single-quoted) state.
        else if (rune == '\'') {
            _switchTo(State::ATTRIBUTE_VALUE_SINGLE_QUOTED);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is a missing-attribute-value parse error. Switch to the data
        // state. Emit the current tag token.
        else if (rune == '>') {
            _raise("missing-attribute-value");
            _switchTo(State::DATA);
            _emit();
        }

        // Anything else
        // Reconsume in the attribute value (unquoted) state.
        else {
            _reconsumeIn(State::ATTRIBUTE_VALUE_UNQUOTED, rune);
        }

        break;
    }

    case State::ATTRIBUTE_VALUE_DOUBLE_QUOTED: {
        // 13.2.5.36 MARK: Attribute value (double-quoted) state
        // Consume the next input character:

        // U+0022 QUOTATION MARK (")
        // Switch to the after attribute value (quoted) state.
        if (rune == '"') {
            _lastAttr().value = _builder.take();
            _switchTo(State::AFTER_ATTRIBUTE_VALUE_QUOTED);
        }

        // U+0026 AMPERSAND (&)
        // Set the return state to the attribute value (double-quoted)
        // state. Switch to the character reference state.
        else if (rune == '&') {
            _returnState = State::ATTRIBUTE_VALUE_DOUBLE_QUOTED;
            _switchTo(State::CHARACTER_REFERENCE);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current attribute's value.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // EOF
        // This is an eof-in-tag parse error. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-tag");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current attribute's
        // value.
        else {
            _builder.append(rune);
        }

        break;
    }

    case State::ATTRIBUTE_VALUE_SINGLE_QUOTED: {
        // 13.2.5.37 MARK: Attribute value (single-quoted) state
        // Consume the next input character:

        // U+0027 APOSTROPHE (')
        // Switch to the after attribute value (quoted) state.
        if (rune == '\'') {
            _lastAttr().value = _builder.take();
            _switchTo(State::AFTER_ATTRIBUTE_VALUE_QUOTED);
        }

        // U+0026 AMPERSAND (&)
        // Set the return state to the attribute value (single-quoted)
        // state. Switch to the character reference state.
        else if (rune == '&') {
            _returnState = State::ATTRIBUTE_VALUE_SINGLE_QUOTED;
            _switchTo(State::CHARACTER_REFERENCE);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current attribute's value.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // EOF
        // This is an eof-in-tag parse error. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-tag");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current attribute's
        // value.
        else {
            _builder.append(rune);
        }

        break;
    }
    case State::ATTRIBUTE_VALUE_UNQUOTED: {
        // 13.2.5.38 MARK: Attribute value (unquoted) state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Switch to the before attribute name state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            _lastAttr().value = _builder.take();
            _switchTo(State::BEFORE_ATTRIBUTE_NAME);
        }

        // U+0026 AMPERSAND (&)
        // Set the return state to the attribute value (unquoted) state.
        // Switch to the character reference state.
        else if (rune == '&') {
            _returnState = State::ATTRIBUTE_VALUE_UNQUOTED;
            _switchTo(State::CHARACTER_REFERENCE);
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current tag token.
        else if (rune == '>') {
            _lastAttr().value = _builder.take();
            _switchTo(State::DATA);
            _emit();
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current attribute's value.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // U+0022 QUOTATION MARK (")
        // U+0027 APOSTROPHE (')
        // U+003C LESS-THAN SIGN (<)
        // U+003D EQUALS SIGN (=)
        // U+0060 GRAVE ACCENT (`)
        // This is an unexpected-character-in-unquoted-attribute-value parse
        // error. Treat it as per the "anything else" entry below.
        else if (rune == '"' or rune == '\'' or rune == '<' or rune == '=' or rune == '`') {
            _raise("unexpected-character-in-unquoted-attribute-value");
            _builder.append(rune);
        }

        // EOF
        // This is an eof-in-tag parse error. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-tag");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current attribute's
        // value.
        else {
            _builder.append(rune);
        }

        break;
    }

    case State::AFTER_ATTRIBUTE_VALUE_QUOTED: {
        // 13.2.5.39 MARK: After attribute value (quoted) state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Switch to the before attribute name state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            _switchTo(State::BEFORE_ATTRIBUTE_NAME);
        }

        // U+002F SOLIDUS (/)
        // Switch to the self-closing start tag state.
        else if (rune == '/') {
            _switchTo(State::SELF_CLOSING_START_TAG);
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current tag token.
        else if (rune == '>') {
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-tag parse error. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-tag");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is a missing-whitespace-between-attributes parse error.
        // Reconsume in the before attribute name state.
        else {
            _raise("missing-whitespace-between-attributes");
            _reconsumeIn(State::BEFORE_ATTRIBUTE_NAME, rune);
        }

        break;
    }

    case State::SELF_CLOSING_START_TAG: {
        // 13.2.5.40 MARK: Self-closing start tag state
        // Consume the next input character:

        // U+003E GREATER-THAN SIGN (>)
        // Set the self-closing flag of the current tag token. Switch to the
        // data state. Emit the current tag token.
        if (rune == '>') {
            _ensure().selfClosing = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-tag parse error. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-tag");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is an unexpected-solidus-in-tag parse error. Reconsume in
        // the before attribute name state.
        else {
            _raise("unexpected-solidus-in-tag");
            _reconsumeIn(State::BEFORE_ATTRIBUTE_NAME, rune);
        }

        break;
    }
    case State::BOGUS_COMMENT: {
        // 13.2.5.41 MARK: Bogus comment state
        // Consume the next input character:

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current comment token.
        if (rune == '>') {
            _ensure(HtmlToken::COMMENT).data = _builder.take();
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // Emit the comment. Emit an end-of-file token.
        else if (isEof) {
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the comment token's data.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // Anything else
        // Append the current input character to the comment token's data.
        else {
            _builder.append(rune);
        }

        break;
    }

    case State::MARKUP_DECLARATION_OPEN: {
        // 13.2.5.42 MARK: Markup declaration open state
        // If the next few characters are:

        _temp.append(rune);
        // Two U+002D HYPHEN-MINUS characters (-)
        // Consume those two characters, create a comment token whose data
        // is the empty string, and switch to the comment start state.
        if (auto r = startWith("--"s, _temp.str()); r != Match::NO) {
            if (r == Match::PARTIAL)
                break;

            _temp.clear();
            _begin(HtmlToken::COMMENT);
            _switchTo(State::COMMENT_START);
        }

        // ASCII case-insensitive match for the word "DOCTYPE"
        // Consume those characters and switch to the DOCTYPE state.

        else if (auto r = startWith("DOCTYPE"s, _temp.str()); r != Match::NO) {
            if (r == Match::PARTIAL)
                break;

            _temp.clear();
            _switchTo(State::DOCTYPE);
        }

        // The string "[CDATA[" (the five uppercase letters "CDATA" with a
        // U+005B LEFT SQUARE BRACKET character before and after) Consume
        // those characters. If there is an adjusted current node and it is
        // not an element in the HTML namespace, then switch to the CDATA
        // section state. Otherwise, this is a cdata-in-html-content parse
        // error. Create a comment token whose data is the "[CDATA[" string.
        // Switch to the bogus comment state.

        else if (auto r = startWith("[CDATA["s, _temp.str()); r != Match::NO) {
            if (r == Match::PARTIAL)
                break;

            // NOSPEC: This is in reallity more complicated
            _temp.clear();
            _switchTo(State::CDATA_SECTION);
        }

        // Anything else
        // This is an incorrectly-opened-comment parse error. Create a
        // comment token whose data is the empty string. Switch to the bogus
        // comment state (don't consume anything in the current state).
        else {
            _temp.clear();
            _raise("incorrectly-opened-comment");
            _begin(HtmlToken::COMMENT);
            _reconsumeIn(State::BOGUS_COMMENT, rune);
        }
        break;
    }

    case State::COMMENT_START: {
        // 13.2.5.43 MARK: Comment start state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the comment start dash state.
        if (rune == '-') {
            _switchTo(State::COMMENT_START_DASH);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is an abrupt-closing-of-empty-comment parse error. Switch to
        // the data state. Emit the current comment token.
        else if (rune == '>') {
            _raise("abrupt-closing-of-empty-comment");
            _switchTo(State::DATA);
            _emit();
        }

        // Anything else
        // Reconsume in the comment state.
        else {
            _reconsumeIn(State::COMMENT, rune);
        }

        break;
    }

    case State::COMMENT_START_DASH: {
        // 13.2.5.44 MARK: Comment start dash state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the comment end state.
        if (rune == '-') {
            _switchTo(State::COMMENT_END);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is an abrupt-closing-of-empty-comment parse error. Switch to
        // the data state. Emit the current comment token.
        else if (rune == '>') {
            _raise("abrupt-closing-of-empty-comment");
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-comment parse error. Emit the current comment
        // token. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-comment");
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append a U+002D HYPHEN-MINUS character (-) to the comment token's
        // data. Reconsume in the comment state.
        else {
            _builder.append('-');
            _reconsumeIn(State::COMMENT, rune);
        }

        break;
    }

    case State::COMMENT: {
        // 13.2.5.45 MARK: Comment state
        // Consume the next input character:

        // U+003C LESS-THAN SIGN (<)
        // Append the current input character to the comment token's data.
        // Switch to the comment less-than sign state.
        if (rune == '<') {
            _builder.append(rune);
            _switchTo(State::COMMENT_LESS_THAN_SIGN);
        }

        // U+002D HYPHEN-MINUS (-)
        // Switch to the comment end dash state.
        else if (rune == '-') {
            _switchTo(State::COMMENT_END_DASH);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the comment token's data.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // EOF
        // This is an eof-in-comment parse error. Emit the current comment
        // token. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-comment");
            _emit();

            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the comment token's data.
        else {
            _builder.append(rune);
        }

        break;
    }

    case State::COMMENT_LESS_THAN_SIGN: {
        // 13.2.5.46 MARK: Comment less-than sign state
        // Consume the next input character:

        // U+0021 EXCLAMATION MARK (!)
        // Append the current input character to the comment token's data.
        // Switch to the comment less-than sign bang state.
        if (rune == '!') {
            _builder.append(rune);
            _switchTo(State::COMMENT_LESS_THAN_SIGN_BANG);
        }

        // U+003C LESS-THAN SIGN (<)
        // Append the current input character to the comment token's data.
        else if (rune == '<') {
            _builder.append(rune);
        }

        // Anything else
        // Reconsume in the comment state.
        else {
            _reconsumeIn(State::COMMENT, rune);
        }

        break;
    }

    case State::COMMENT_LESS_THAN_SIGN_BANG: {
        // 13.2.5.47 MARK: Comment less-than sign bang state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the comment less-than sign bang dash state.
        if (rune == '-') {
            _switchTo(State::COMMENT_LESS_THAN_SIGN_BANG_DASH);
        }

        // Anything else
        // Reconsume in the comment state.
        else {
            _reconsumeIn(State::COMMENT, rune);
        }

        break;
    }

    case State::COMMENT_LESS_THAN_SIGN_BANG_DASH: {
        // 13.2.5.48 MARK: Comment less-than sign bang dash state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the comment less-than sign bang dash dash state.
        if (rune == '-') {
            _switchTo(State::COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH);
        }

        // Anything else
        // Reconsume in the comment end dash state.
        else {
            _reconsumeIn(State::COMMENT_END_DASH, rune);
        }

        break;
    }

    case State::COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH: {
        // 13.2.5.49 MARK: Comment less-than sign bang dash dash state
        // Consume the next input character:

        // U+003E GREATER-THAN SIGN (>)
        // EOF
        // Reconsume in the comment end state.
        if (rune == '>' or isEof) {
            _reconsumeIn(State::COMMENT_END, rune);
        }

        // Anything else
        // This is a nested-comment parse error. Reconsume in the comment
        // end state.
        else {
            _raise("nested-comment");
            _reconsumeIn(State::COMMENT_END, rune);
        }

        break;
    }

    case State::COMMENT_END_DASH: {
        // 13.2.5.50 MARK: Comment end dash state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Switch to the comment end state.
        if (rune == '-') {
            _switchTo(State::COMMENT_END);
        }

        // EOF
        // This is an eof-in-comment parse error. Emit the current comment
        // token. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-comment");
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append a U+002D HYPHEN-MINUS character (-) to the comment token's
        // data. Reconsume in the comment state.
        else {
            _builder.append('-');
            _reconsumeIn(State::COMMENT, rune);
        }

        break;
    }

    case State::COMMENT_END: {
        // 13.2.5.51 MARK: Comment end state
        // Consume the next input character:

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current comment token.
        if (rune == '>') {
            _switchTo(State::DATA);
            _emit();
        }

        // U+0021 EXCLAMATION MARK (!)
        // Switch to the comment end bang state.
        else if (rune == '!') {
            _switchTo(State::COMMENT_END_BANG);
        }

        // U+002D HYPHEN-MINUS (-)
        // Append a U+002D HYPHEN-MINUS character (-) to the comment token's
        // data.
        else if (rune == '-') {
            _builder.append('-');
        }

        // EOF
        // This is an eof-in-comment parse error. Emit the current comment
        // token. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-comment");
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append two U+002D HYPHEN-MINUS characters (-) to the comment
        // token's data. Reconsume in the comment state.
        else {
            _builder.append('-');
            _builder.append('-');
            _reconsumeIn(State::COMMENT, rune);
        }

        break;
    }

    case State::COMMENT_END_BANG: {
        // 13.2.5.52 MARK: Comment end bang state
        // Consume the next input character:

        // U+002D HYPHEN-MINUS (-)
        // Append two U+002D HYPHEN-MINUS characters (-) and a U+0021
        // EXCLAMATION MARK character (!) to the comment token's data.
        // Switch to the comment end dash state.
        if (rune == '-') {
            _builder.append('-');
            _builder.append('-');
            _builder.append('!');
            _switchTo(State::COMMENT_END_DASH);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is an incorrectly-closed-comment parse error. Switch to the
        // data state. Emit the current comment token.
        else if (rune == '>') {
            _raise("incorrectly-closed-comment");
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-comment parse error. Emit the current comment
        // token. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-comment");
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append two U+002D HYPHEN-MINUS characters (-) and a U+0021
        // EXCLAMATION MARK character (!) to the comment token's data.
        // Reconsume in the comment state.
        else {
            _builder.append('-');
            _builder.append('-');
            _builder.append('!');
            _reconsumeIn(State::COMMENT, rune);
        }

        break;
    }

    case State::DOCTYPE: {
        // 13.2.5.53 MARK: DOCTYPE state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Switch to the before DOCTYPE name state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            _switchTo(State::BEFORE_DOCTYPE_NAME);
        }

        // U+003E GREATER-THAN SIGN (>)
        // Reconsume in the before DOCTYPE name state.
        else if (rune == '>') {
            _reconsumeIn(State::BEFORE_DOCTYPE_NAME, rune);
        }

        // EOF
        // This is an eof-in-doctype parse error. Create a new DOCTYPE
        // token. Set its force-quirks flag to on. Emit the current token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _begin(HtmlToken::DOCTYPE);
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is a missing-whitespace-before-doctype-name parse error.
        // Reconsume in the before DOCTYPE name state.
        else {
            _raise("missing-whitespace-before-doctype-name");
            _reconsumeIn(State::BEFORE_DOCTYPE_NAME, rune);
        }

        break;
    }

    case State::BEFORE_DOCTYPE_NAME: {
        // 13.2.5.54 MARK: Before DOCTYPE name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // ASCII upper alpha
        // Create a new DOCTYPE token. Set the token's name to the lowercase
        // version of the current input character (add 0x0020 to the
        // character's code point). Switch to the DOCTYPE name state.
        else if (isAsciiUpper(rune)) {
            _begin(HtmlToken::DOCTYPE);
            _builder.append(toAsciiLower(rune));
            _switchTo(State::DOCTYPE_NAME);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Create a new
        // DOCTYPE token. Set the token's name to a U+FFFD REPLACEMENT
        // CHARACTER character. Switch to the DOCTYPE name state.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _begin(HtmlToken::DOCTYPE);
            _builder.append(0xFFFD);
            _switchTo(State::DOCTYPE_NAME);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is a missing-doctype-name parse error. Create a new DOCTYPE
        // token. Set its force-quirks flag to on. Switch to the data state.
        // Emit the current token.
        else if (rune == '>') {
            _raise("missing-doctype-name");
            _begin(HtmlToken::DOCTYPE);
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Create a new DOCTYPE
        // token. Set its force-quirks flag to on. Emit the current token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _begin(HtmlToken::DOCTYPE);
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Create a new DOCTYPE token. Set the token's name to the current
        // input character. Switch to the DOCTYPE name state.
        else {
            _begin(HtmlToken::DOCTYPE);
            _builder.append(rune);
            _switchTo(State::DOCTYPE_NAME);
        }

        break;
    }

    case State::DOCTYPE_NAME: {
        // 13.2.5.55 MARK: DOCTYPE name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Switch to the after DOCTYPE name state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            _ensure(HtmlToken::DOCTYPE).name = _builder.take();
            _switchTo(State::AFTER_DOCTYPE_NAME);
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _ensure(HtmlToken::DOCTYPE).name = _builder.take();
            _switchTo(State::DATA);
            _emit();
        }

        // ASCII upper alpha
        // Append the lowercase version of the current input character (add
        // 0x0020 to the character's code point) to the current DOCTYPE
        // token's name.
        else if (isAsciiUpper(rune)) {
            _builder.append(toAsciiLower(rune));
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current DOCTYPE token's
        // name.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current DOCTYPE token's
        // name.
        else if (isAsciiLower(rune)) {
            _builder.append(rune);
        }

        break;
    }

    case State::AFTER_DOCTYPE_NAME: {
        // 13.2.5.56 MARK: After DOCTYPE name state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // If the six characters starting from the current input character
        // are an ASCII case-insensitive match for the word "PUBLIC", then
        // consume those characters and switch to the after DOCTYPE public
        // keyword state.
        else if (rune == 'p' or rune == 'P') {
            _switchTo(State::AFTER_DOCTYPE_PUBLIC_KEYWORD);
        }

        // Otherwise, if the six characters starting from the current input
        // character are an ASCII case-insensitive match for the word
        // "SYSTEM", then consume those characters and switch to the after
        // DOCTYPE system keyword state.
        else if (rune == 's' or rune == 'S') {
            _switchTo(State::AFTER_DOCTYPE_SYSTEM_KEYWORD);
        }

        // Otherwise, this is an
        // invalid-character-sequence-after-doctype-name parse error. Set
        // the current DOCTYPE token's force-quirks flag to on. Reconsume in
        // the bogus DOCTYPE state.
        else {
            _raise("invalid-character-sequence-after-doctype-name");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _reconsumeIn(State::BOGUS_DOCTYPE, rune);
        }

        break;
    }

    case State::AFTER_DOCTYPE_PUBLIC_KEYWORD: {
        // 13.2.5.57 MARK: After DOCTYPE public keyword state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Switch to the before DOCTYPE public identifier state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            _switchTo(State::BEFORE_DOCTYPE_PUBLIC_IDENTIFIER);
        }

        // U+0022 QUOTATION MARK (")
        // This is a missing-whitespace-after-doctype-public-keyword parse
        // error. Set the current DOCTYPE token's public identifier to the
        // empty string (not missing), then switch to the DOCTYPE public
        // identifier (double-quoted) state.
        else if (rune == '"') {
            _raise("missing-whitespace-after-doctype-public-keyword");
            _switchTo(State::DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED);
        }

        // U+0027 APOSTROPHE (')
        // This is a missing-whitespace-after-doctype-public-keyword parse
        // error. Set the current DOCTYPE token's public identifier to the
        // empty string (not missing), then switch to the DOCTYPE public
        // identifier (single-quoted) state.
        else if (rune == '\'') {
            _raise("missing-whitespace-after-doctype-public-keyword");
            _switchTo(State::DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is a missing-doctype-public-identifier parse error. Set the
        // current DOCTYPE token's force-quirks flag to on. Switch to the
        // data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _raise("missing-doctype-public-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is a missing-quote-before-doctype-public-identifier parse
        // error. Set the current DOCTYPE token's force-quirks flag to on.
        // Reconsume in the bogus DOCTYPE state.
        else {
            _raise("missing-quote-before-doctype-public-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _reconsumeIn(State::BOGUS_DOCTYPE, rune);
        }

        break;
    }

    case State::BEFORE_DOCTYPE_PUBLIC_IDENTIFIER: {
        // 13.2.5.58 MARK: Before DOCTYPE public identifier state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // U+0022 QUOTATION MARK (")
        // Set the current DOCTYPE token's public identifier to the empty
        // string (not missing), then switch to the DOCTYPE public
        // identifier (double-quoted) state.
        else if (rune == '"') {
            _switchTo(State::DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED);
        }

        // U+0027 APOSTROPHE (')
        // Set the current DOCTYPE token's public identifier to the empty
        // string (not missing), then switch to the DOCTYPE public
        // identifier (single-quoted) state.
        else if (rune == '\'') {
            _switchTo(State::DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is a missing-doctype-public-identifier parse error. Set the
        // current DOCTYPE token's force-quirks flag to on. Switch to the
        // data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _raise("missing-doctype-public-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is a missing-quote-before-doctype-public-identifier parse
        // error. Set the current DOCTYPE token's force-quirks flag to on.
        // Reconsume in the bogus DOCTYPE state.
        else {
            _raise("missing-quote-before-doctype-public-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _reconsumeIn(State::BOGUS_DOCTYPE, rune);
        }

        break;
    }

    case State::DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED: {
        // 13.2.5.59 MARK: DOCTYPE public identifier (double-quoted) state
        // Consume the next input character:

        // U+0022 QUOTATION MARK (")
        // Switch to the after DOCTYPE public identifier state.
        if (rune == '"') {
            _ensure(HtmlToken::DOCTYPE).publicIdent = _builder.take();
            _switchTo(State::AFTER_DOCTYPE_PUBLIC_IDENTIFIER);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current DOCTYPE token's
        // public identifier.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is an abrupt-doctype-public-identifier parse error. Set the
        // current DOCTYPE token's force-quirks flag to on. Switch to the
        // data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _raise("abrupt-doctype-public-identifier");
            _ensure(HtmlToken::DOCTYPE).publicIdent = _builder.take();
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current DOCTYPE token's
        // public identifier.
        else {
            _builder.append(rune);
        }

        break;
    }

    case State::DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED: {
        // 13.2.5.60 MARK: DOCTYPE public identifier (single-quoted) state
        // Consume the next input character:

        // U+0027 APOSTROPHE (')
        // Switch to the after DOCTYPE public identifier state.
        if (rune == '\'') {
            _ensure(HtmlToken::DOCTYPE).publicIdent = _builder.take();
            _switchTo(State::AFTER_DOCTYPE_PUBLIC_IDENTIFIER);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current DOCTYPE token's
        // public identifier.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is an abrupt-doctype-public-identifier parse error. Set the
        // current DOCTYPE token's force-quirks flag to on. Switch to the
        // data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _raise("abrupt-doctype-public-identifier");
            _ensure(HtmlToken::DOCTYPE).publicIdent = _builder.take();
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current DOCTYPE token's
        // public identifier.
        else {
            _builder.append(rune);
        }
        break;
    }

    case State::AFTER_DOCTYPE_PUBLIC_IDENTIFIER: {
        // 13.2.5.61 MARK: After DOCTYPE public identifier state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Switch to the between DOCTYPE public and system identifiers
        // state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            _switchTo(State::BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS);
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _switchTo(State::DATA);
            _emit();
        }

        // U+0022 QUOTATION MARK (")
        // This is a
        // missing-whitespace-between-doctype-public-and-system-identifiers
        // parse error. Set the current DOCTYPE token's system identifier to
        // the empty string (not missing), then switch to the DOCTYPE system
        // identifier (double-quoted) state.
        else if (rune == '"') {
            _raise("missing-whitespace-between-doctype-public-and-system-identifiers");
            _switchTo(State::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED);
        }

        // U+0027 APOSTROPHE (')
        // This is a
        // missing-whitespace-between-doctype-public-and-system-identifiers
        // parse error. Set the current DOCTYPE token's system identifier to
        // the empty string (not missing), then switch to the DOCTYPE system
        // identifier (single-quoted) state.
        else if (rune == '\'') {
            _raise("missing-whitespace-between-doctype-public-and-system-identifiers");
            _switchTo(State::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED);
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is a missing-quote-before-doctype-system-identifier parse
        // error. Set the current DOCTYPE token's force-quirks flag to on.
        // Reconsume in the bogus DOCTYPE state.
        else {
            _raise("missing-quote-before-doctype-system-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _reconsumeIn(State::BOGUS_DOCTYPE, rune);
        }

        break;
    }

    case State::BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS: {
        // 13.2.5.62 MARK: Between DOCTYPE public and system identifiers state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _switchTo(State::DATA);
            _emit();
        }

        // U+0022 QUOTATION MARK (")
        // Set the current DOCTYPE token's system identifier to the empty
        // string (not missing), then switch to the DOCTYPE system
        // identifier (double-quoted) state.
        else if (rune == '"') {
            _switchTo(State::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED);
        }

        // U+0027 APOSTROPHE (')
        // Set the current DOCTYPE token's system identifier to the empty
        // string (not missing), then switch to the DOCTYPE system
        // identifier (single-quoted) state.
        else if (rune == '\'') {
            _switchTo(State::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED);
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is a missing-quote-before-doctype-system-identifier parse
        // error. Set the current DOCTYPE token's force-quirks flag to on.
        // Reconsume in the bogus DOCTYPE state.
        else {
            _raise("missing-quote-before-doctype-system-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _reconsumeIn(State::BOGUS_DOCTYPE, rune);
        }

        break;
    }

    case State::AFTER_DOCTYPE_SYSTEM_KEYWORD: {
        // 13.2.5.63 MARK: After DOCTYPE system keyword state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Switch to the before DOCTYPE system identifier state.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            _switchTo(State::BEFORE_DOCTYPE_SYSTEM_IDENTIFIER);
        }

        // U+0022 QUOTATION MARK (")
        // This is a missing-whitespace-after-doctype-system-keyword parse
        // error. Set the current DOCTYPE token's system identifier to the
        // empty string (not missing), then switch to the DOCTYPE system
        // identifier (double-quoted) state.
        else if (rune == '"') {
            _raise("missing-whitespace-after-doctype-system-keyword");
            _switchTo(State::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED);
        }

        // U+0027 APOSTROPHE (')
        // This is a missing-whitespace-after-doctype-system-keyword parse
        // error. Set the current DOCTYPE token's system identifier to the
        // empty string (not missing), then switch to the DOCTYPE system
        // identifier (single-quoted) state.
        else if (rune == '\'') {
            _raise("missing-whitespace-after-doctype-system-keyword");
            _switchTo(State::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is a missing-doctype-system-identifier parse error. Set the
        // current DOCTYPE token's force-quirks flag to on. Switch to the
        // data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _raise("missing-doctype-system-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is a missing-quote-before-doctype-system-identifier parse
        // error. Set the current DOCTYPE token's force-quirks flag to on.
        // Reconsume in the bogus DOCTYPE state.
        else {
            _raise("missing-quote-before-doctype-system-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _reconsumeIn(State::BOGUS_DOCTYPE, rune);
        }

        break;
    }

    case State::BEFORE_DOCTYPE_SYSTEM_IDENTIFIER: {
        // 13.2.5.64 MARK: Before DOCTYPE system identifier state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // U+0022 QUOTATION MARK (")
        // Set the current DOCTYPE token's system identifier to the empty
        // string (not missing), then switch to the DOCTYPE system
        // identifier (double-quoted) state.
        else if (rune == '"') {
            _switchTo(State::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED);
        }

        // U+0027 APOSTROPHE (')
        // Set the current DOCTYPE token's system identifier to the empty
        // string (not missing), then switch to the DOCTYPE system
        // identifier (single-quoted) state.
        else if (rune == '\'') {
            _switchTo(State::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is a missing-doctype-system-identifier parse error. Set the
        // current DOCTYPE token's force-quirks flag to on. Switch to the
        // data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _raise("missing-doctype-system-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is a missing-quote-before-doctype-system-identifier parse
        // error. Set the current DOCTYPE token's force-quirks flag to on.
        // Reconsume in the bogus DOCTYPE state.
        else {
            _raise("missing-quote-before-doctype-system-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _reconsumeIn(State::BOGUS_DOCTYPE, rune);
        }

        break;
    }

    case State::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED: {
        // 13.2.5.65 MARK: DOCTYPE system identifier (double-quoted) state
        // Consume the next input character:

        // U+0022 QUOTATION MARK (")
        // Switch to the after DOCTYPE system identifier state.
        if (rune == '"') {
            _switchTo(State::AFTER_DOCTYPE_SYSTEM_IDENTIFIER);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current DOCTYPE token's
        // system identifier.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is an abrupt-doctype-system-identifier parse error. Set the
        // current DOCTYPE token's force-quirks flag to on. Switch to the
        // data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _raise("abrupt-doctype-system-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current DOCTYPE token's
        // system identifier.
        else {
            _builder.append(rune);
        }

        break;
    }

    case State::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED: {
        // 13.2.5.66 MARK: DOCTYPE system identifier (single-quoted) state
        // Consume the next input character:

        // U+0027 APOSTROPHE (')
        // Switch to the after DOCTYPE system identifier state.
        if (rune == '\'') {
            _switchTo(State::AFTER_DOCTYPE_SYSTEM_IDENTIFIER);
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Append a U+FFFD
        // REPLACEMENT CHARACTER character to the current DOCTYPE token's
        // system identifier.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            _builder.append(0xFFFD);
        }

        // U+003E GREATER-THAN SIGN (>)
        // This is an abrupt-doctype-system-identifier parse error. Set the
        // current DOCTYPE token's force-quirks flag to on. Switch to the
        // data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _raise("abrupt-doctype-system-identifier");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Append the current input character to the current DOCTYPE token's
        // system identifier.
        else if (rune != 0) {
            _builder.append(rune);
        }

        break;
    }

    case State::AFTER_DOCTYPE_SYSTEM_IDENTIFIER: {
        // 13.2.5.67 MARK: After DOCTYPE system identifier state
        // Consume the next input character:

        // U+0009 CHARACTER TABULATION (tab)
        // U+000A LINE FEED (LF)
        // U+000C FORM FEED (FF)
        // U+0020 SPACE
        // Ignore the character.
        if (rune == '\t' or rune == '\n' or rune == '\f' or rune == ' ') {
            // Ignore
        }

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the current DOCTYPE token.
        else if (rune == '>') {
            _switchTo(State::DATA);
            _emit();
        }

        // EOF
        // This is an eof-in-doctype parse error. Set the current DOCTYPE
        // token's force-quirks flag to on. Emit the current DOCTYPE token.
        // Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-doctype");
            _ensure(HtmlToken::DOCTYPE).forceQuirks = true;
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // This is an unexpected-character-after-doctype-system-identifier
        // parse error. Reconsume in the bogus DOCTYPE state. (This does not
        // set the current DOCTYPE token's force-quirks flag to on.)
        else {
            _raise("unexpected-character-after-doctype-system-identifier");
            _reconsumeIn(State::BOGUS_DOCTYPE, rune);
        }

        break;
    }

    case State::BOGUS_DOCTYPE: {
        // 13.2.5.68 MARK: Bogus DOCTYPE state
        // Consume the next input character:

        // U+003E GREATER-THAN SIGN (>)
        // Switch to the data state. Emit the DOCTYPE token.
        if (rune == '>') {
            _switchTo(State::DATA);
            _emit();
        }

        // U+0000 NULL
        // This is an unexpected-null-character parse error. Ignore the
        // character.
        else if (rune == 0) {
            _raise("unexpected-null-character");
            // Ignore
        }

        // EOF
        // Emit the DOCTYPE token. Emit an end-of-file token.
        else if (isEof) {
            _emit();
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Ignore the character.
        else {
            // Ignore
        }

        break;
    }

    case State::CDATA_SECTION: {
        // 13.2.5.69 MARK: CDATA section state
        // Consume the next input character:

        // U+005D RIGHT SQUARE BRACKET (])
        // Switch to the CDATA section bracket state.
        if (rune == ']') {
            _switchTo(State::CDATA_SECTION_BRACKET);
        }

        // EOF
        // This is an eof-in-cdata parse error. Emit an end-of-file token.
        else if (isEof) {
            _raise("eof-in-cdata");
            _begin(HtmlToken::END_OF_FILE);
            _emit();
        }

        // Anything else
        // Emit the current input character as a character token.
        // U+0000 NULL characters are handled in the tree construction
        // stage, as part of the in foreign content insertion mode, which is
        // the only place where CDATA sections can appear.
        else {
            _emit(rune);
        }

        break;
    }

    case State::CDATA_SECTION_BRACKET: {
        // 13.2.5.70 MARK: CDATA section bracket state
        // Consume the next input character:

        // U+005D RIGHT SQUARE BRACKET (])
        // Switch to the CDATA section end state.
        if (rune == ']') {
            _switchTo(State::CDATA_SECTION_END);
        }

        // Anything else
        // Emit a U+005D RIGHT SQUARE BRACKET character token. Reconsume in
        // the CDATA section state.
        else {
            _emit(']');
            _reconsumeIn(State::CDATA_SECTION, rune);
        }

        break;
    }

    case State::CDATA_SECTION_END: {
        // 13.2.5.71 MARK: CDATA section end state
        // Consume the next input character:

        // U+005D RIGHT SQUARE BRACKET (])
        // Emit a U+005D RIGHT SQUARE BRACKET character token.
        if (rune == ']') {
            _emit(']');
        }

        // U+003E GREATER-THAN SIGN character
        // Switch to the data state.
        else if (rune == '>') {
            _switchTo(State::DATA);
        }

        // Anything else
        // Emit two U+005D RIGHT SQUARE BRACKET character tokens. Reconsume
        // in the CDATA section state.
        else {
            _emit(']');
            _emit(']');
            _reconsumeIn(State::CDATA_SECTION, rune);
        }

        break;
    }

    case State::CHARACTER_REFERENCE: {
        // 13.2.5.72 MARK: Character reference state
        // Set the temporary buffer to the empty string. Append a U+0026

        // AMPERSAND (&) character to the temporary buffer. Consume the next
        // input character:
        _temp.clear();
        _temp.append('&');

        // ASCII alphanumeric
        // Reconsume in the named character reference state.
        if (isAsciiAlphaNum(rune)) {
            _reconsumeIn(State::NAMED_CHARACTER_REFERENCE, rune);
        }

        // U+0023 NUMBER SIGN (#)
        // Append the current input character to the temporary buffer.
        // Switch to the numeric character reference state.

        else if (rune == '#') {
            _temp.append('#');
            _switchTo(State::NUMERIC_CHARACTER_REFERENCE);
        }

        // Anything else
        // Flush code points consumed as a character reference. Reconsume in
        // the return state.
        else {
            _flushCodePointsConsumedAsACharacterReference();
            _reconsumeIn(_returnState, rune);
        }

        break;
    }

    case State::NAMED_CHARACTER_REFERENCE: {
        notImplemented();
        // 13.2.5.73 MARK: Named character reference state

        // Consume the maximum number of characters possible, where the
        // consumed characters are one of the identifiers in the first
        // column of the named character references table. Append each
        // character to the temporary buffer when it's consumed.

        // If there is a match
        // If the character reference was consumed as part of an attribute,
        // and the last character matched is not a U+003B SEMICOLON
        // character (;), and the next input character is either a U+003D
        // EQUALS SIGN character (=) or an ASCII alphanumeric, then, for
        // historical reasons, flush code points consumed as a character
        // reference and switch to the return state.

        // Otherwise:

        // If the last character matched is not a U+003B SEMICOLON character
        // (;), then this is a missing-semicolon-after-character-reference
        // parse error.

        // Set the temporary buffer to the empty string. Append one or two
        // characters corresponding to the character reference name (as
        // given by the second column of the named character references
        // table) to the temporary buffer.

        // Flush code points consumed as a character reference. Switch to
        // the return state. Otherwise Flush code points consumed as a
        // character reference. Switch to the ambiguous ampersand state. If
        // the markup contains (not in an attribute) the string I'm &notit;
        // I tell you, the character reference is parsed as "not", as in,
        // I'm ¬it; I tell you (and this is a parse error). But if the
        // markup was I'm &notin; I tell you, the character reference would
        // be parsed as "notin;", resulting in I'm ∉ I tell you (and no
        // parse error).

        // However, if the markup contains the string I'm &notit; I tell you
        // in an attribute, no character reference is parsed and string
        // remains intact (and there is no parse error).
        break;
    }

    case State::AMBIGUOUS_AMPERSAND: {
        // 13.2.5.74 MARK: Ambiguous ampersand state
        // Consume the next input character:

        // ASCII alphanumeric
        // If the character reference was consumed as part of an attribute,
        // then append the current input character to the current
        // attribute's value. Otherwise, emit the current input character as
        // a character token.
        if (isAsciiAlphaNum(rune)) {
            if (_returnState == State::ATTRIBUTE_VALUE_DOUBLE_QUOTED ||
                _returnState == State::ATTRIBUTE_VALUE_SINGLE_QUOTED ||
                _returnState == State::ATTRIBUTE_VALUE_UNQUOTED) {
                _builder.append(rune);
            } else {
                _emit(rune);
            }
        }

        // U+003B SEMICOLON (;)
        // This is an unknown-named-character-reference parse error.
        // Reconsume in the return state.
        else if (rune == ';') {
            _raise("unknown-named-character-reference");
            _reconsumeIn(_returnState, rune);
        }

        // Anything else
        // Reconsume in the return state.
        else {
            _reconsumeIn(_returnState, rune);
        }

        break;
    }

    case State::NUMERIC_CHARACTER_REFERENCE: {
        // 13.2.5.75 MARK: Numeric character reference state
        // Set the character reference code to zero (0).

        // Consume the next input character:

        // U+0078 LATIN SMALL LETTER X
        // U+0058 LATIN CAPITAL LETTER X
        // Append the current input character to the temporary buffer.
        // Switch to the hexadecimal character reference start state.
        if (rune == 'x' or rune == 'X') {
            _switchTo(State::HEXADECIMAL_CHARACTER_REFERENCE_START);
        }

        // Anything else
        // Reconsume in the decimal character reference start state.
        else {
            _reconsumeIn(State::DECIMAL_CHARACTER_REFERENCE_START, rune);
        }

        break;
    }

    case State::HEXADECIMAL_CHARACTER_REFERENCE_START: {
        // 13.2.5.76 MARK: Hexadecimal character reference start state
        // Consume the next input character:

        // ASCII hex digit
        // Reconsume in the hexadecimal character reference state.
        if (isAsciiHexDigit(rune)) {
            _reconsumeIn(State::HEXADECIMAL_CHARACTER_REFERENCE, rune);
        }

        // Anything else
        // This is an absence-of-digits-in-numeric-character-reference parse
        // error. Flush code points consumed as a character reference.
        // Reconsume in the return state.
        else {
            _raise("absence-of-digits-in-numeric-character-reference");
            _flushCodePointsConsumedAsACharacterReference();
            _reconsumeIn(_returnState, rune);
        }

        break;
    }

    case State::DECIMAL_CHARACTER_REFERENCE_START: {
        // 13.2.5.77 MARK: Decimal character reference start state
        // Consume the next input character:

        // ASCII digit
        // Reconsume in the decimal character reference state.
        if (isAsciiDigit(rune)) {
            _reconsumeIn(State::DECIMAL_CHARACTER_REFERENCE, rune);
        }

        // Anything else
        // This is an absence-of-digits-in-numeric-character-reference parse
        // error. Flush code points consumed as a character reference.
        // Reconsume in the return state.
        else {
            _raise("absence-of-digits-in-numeric-character-reference");
            _flushCodePointsConsumedAsACharacterReference();
            _reconsumeIn(_returnState, rune);
        }

        break;
    }

    case State::HEXADECIMAL_CHARACTER_REFERENCE: {
        // 13.2.5.78 MARK: Hexadecimal character reference state
        // Consume the next input character:

        // ASCII digit
        // Multiply the character reference code by 16. Add a numeric
        // version of the current input character (subtract 0x0030 from the
        // character's code point) to the character reference code.
        if (isAsciiDigit(rune)) {
            _currChar = _currChar * 16 + rune - '0';
        }

        // ASCII upper hex digit
        // Multiply the character reference code by 16. Add a numeric
        // version of the current input character as a hexadecimal digit
        // (subtract 0x0037 from the character's code point) to the
        // character reference code.
        else if (isAsciiUpper(rune)) {
            _currChar = _currChar * 16 + rune - '7';
        }

        // ASCII lower hex digit
        // Multiply the character reference code by 16. Add a numeric
        // version of the current input character as a hexadecimal digit
        // (subtract 0x0057 from the character's code point) to the
        // character reference code.
        else if (isAsciiLower(rune)) {
            _currChar = _currChar * 16 + rune - 'W';
        }

        // U+003B SEMICOLON
        // Switch to the numeric character reference end state.
        else if (rune == ';') {
            _switchTo(State::NUMERIC_CHARACTER_REFERENCE_END);
        }

        // Anything else
        // This is a missing-semicolon-after-character-reference parse
        // error. Reconsume in the numeric character reference end state.
        else {
            _raise("missing-semicolon-after-character-reference");
            _reconsumeIn(State::NUMERIC_CHARACTER_REFERENCE_END, rune);
        }

        break;
    }

    case State::DECIMAL_CHARACTER_REFERENCE: {
        // 13.2.5.79 MARK: Decimal character reference state
        // Consume the next input character:

        // ASCII digit
        // Multiply the character reference code by 10. Add a numeric
        // version of the current input character (subtract 0x0030 from the
        // character's code point) to the character reference code.
        if (isAsciiDigit(rune)) {
            _currChar = _currChar * 10 + rune - '0';
        }

        // U+003B SEMICOLON
        // Switch to the numeric character reference end state.
        else if (rune == ';') {
            _switchTo(State::NUMERIC_CHARACTER_REFERENCE_END);
        }

        // Anything else
        // This is a missing-semicolon-after-character-reference parse
        // error. Reconsume in the numeric character reference end state.
        else {
            _raise("missing-semicolon-after-character-reference");
            _reconsumeIn(State::NUMERIC_CHARACTER_REFERENCE_END, rune);
        }

        break;
    }

    case State::NUMERIC_CHARACTER_REFERENCE_END: {
        // 13.2.5.80 MARK: Numeric character reference end state
        // Check the character reference code:

        // If the number is 0x00, then this is a null-character-reference
        // parse error. Set the character reference code to 0xFFFD.
        if (_currChar == 0) {
            _raise("null-character-reference");
            _currChar = 0xFFFD;
        }

        // If the number is greater than 0x10FFFF, then this is a
        // character-reference-outside-unicode-range parse error. Set the
        // character reference code to 0xFFFD.
        else if (isUnicode(rune)) {
            _raise("character-reference-outside-unicode-range");
            _currChar = 0xFFFD;
        }

        // If the number is a surrogate, then this is a
        // surrogate-character-reference parse error. Set the character
        // reference code to 0xFFFD.
        else if (isUnicodeSurrogate(rune)) {
            _raise("surrogate-character-reference");
            _currChar = 0xFFFD;
        }

        // If the number is a noncharacter, then this is a
        // noncharacter-character-reference parse error.
        else if ((0xFDD0 <= _currChar and _currChar <= 0xFDEF) or (_currChar & 0xFFFF) == 0xFFFE or (_currChar & 0xFFFF) == 0xFFFF) {
            _raise("noncharacter-character-reference");
        }

        // If the number is 0x0D, or a control that's not ASCII whitespace,
        // then this is a control-character-reference parse error.
        else if ((_currChar & 0xFFFF) == 0x000D or isAsciiBlank(_currChar)) {
            _raise("control-character-reference");
        }

        // If the number is one of the numbers in the first column of the
        // following table, then find the row with that number in the first
        // column, and set the character reference code to the number in the
        // second column of that row.

        // Number	Code point
        // 0x80	0x20AC	EURO SIGN (€)
        // 0x82	0x201A	SINGLE LOW-9 QUOTATION MARK (‚)
        // 0x83	0x0192	LATIN SMALL LETTER F WITH HOOK (ƒ)
        // 0x84	0x201E	DOUBLE LOW-9 QUOTATION MARK („)
        // 0x85	0x2026	HORIZONTAL ELLIPSIS (…)
        // 0x86	0x2020	DAGGER (†)
        // 0x87	0x2021	DOUBLE DAGGER (‡)
        // 0x88	0x02C6	MODIFIER LETTER CIRCUMFLEX ACCENT (ˆ)
        // 0x89	0x2030	PER MILLE SIGN (‰)
        // 0x8A	0x0160	LATIN CAPITAL LETTER S WITH CARON (Š)
        // 0x8B	0x2039	SINGLE LEFT-POINTING ANGLE QUOTATION MARK (‹)
        // 0x8C	0x0152	LATIN CAPITAL LIGATURE OE (Œ)
        // 0x8E	0x017D	LATIN CAPITAL LETTER Z WITH CARON (Ž)
        // 0x91	0x2018	LEFT SINGLE QUOTATION MARK (‘)
        // 0x92	0x2019	RIGHT SINGLE QUOTATION MARK (’)
        // 0x93	0x201C	LEFT DOUBLE QUOTATION MARK (“)
        // 0x94	0x201D	RIGHT DOUBLE QUOTATION MARK (”)
        // 0x95	0x2022	BULLET (•)
        // 0x96	0x2013	EN DASH (–)
        // 0x97	0x2014	EM DASH (—)
        // 0x98	0x02DC	SMALL TILDE (˜)
        // 0x99	0x2122	TRADE MARK SIGN (™)
        // 0x9A	0x0161	LATIN SMALL LETTER S WITH CARON (š)
        // 0x9B	0x203A	SINGLE RIGHT-POINTING ANGLE QUOTATION MARK (›)
        // 0x9C	0x0153	LATIN SMALL LIGATURE OE (œ)
        // 0x9E	0x017E	LATIN SMALL LETTER Z WITH CARON (ž)
        // 0x9F	0x0178	LATIN CAPITAL LETTER Y WITH DIAERESIS (Ÿ)

        Array const CONV = {
            Cons{0x80u, 0x20ACuz},
            Cons{0x82u, 0x201Auz},
            Cons{0x83u, 0x0192uz},
            Cons{0x84u, 0x201Euz},
            Cons{0x85u, 0x2026uz},
            Cons{0x86u, 0x2020uz},
            Cons{0x87u, 0x2021uz},
            Cons{0x88u, 0x02C6uz},
            Cons{0x89u, 0x2030uz},
            Cons{0x8Au, 0x0160uz},
            Cons{0x8Bu, 0x2039uz},
            Cons{0x8Cu, 0x0152uz},
            Cons{0x8Eu, 0x017Duz},
            Cons{0x91u, 0x2018uz},
            Cons{0x92u, 0x2019uz},
            Cons{0x93u, 0x201Cuz},
            Cons{0x94u, 0x201Duz},
            Cons{0x95u, 0x2022uz},
            Cons{0x96u, 0x2013uz},
            Cons{0x97u, 0x2014uz},
            Cons{0x98u, 0x02DCuz},
            Cons{0x99u, 0x2122uz},
            Cons{0x9Au, 0x0161uz},
            Cons{0x9Bu, 0x203Auz},
            Cons{0x9Cu, 0x0153uz},
            Cons{0x9Eu, 0x017Euz},
            Cons{0x9Fu, 0x0178uz},
        };

        for (auto &conv : CONV)
            if (conv.car == _currChar) {
                _currChar = conv.cdr;
                break;
            }

        // Set the temporary buffer to the empty string. Append a code point
        // equal to the character reference code to the temporary buffer.
        // Flush code points consumed as a character reference. Switch to
        // the return state.

        _temp.clear();
        _temp.append(_currChar);
        _flushCodePointsConsumedAsACharacterReference();
        _switchTo(_returnState);

        break;
    }

    default:
        panic("unknown-state");
        break;
    }
}

// MARK: Parser ----------------------------------------------------------------

// 13.2.2 MARK: Parse errors
// https://html.spec.whatwg.org/multipage/parsing.html#parse-errors

void HtmlParser::_raise(Str msg) {
    logError("{}: {}", _insertionMode, msg);
}

// 13.2.4.2 MARK: The stack of open elements
// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements

// https://html.spec.whatwg.org/multipage/parsing.html#special
bool isSpecial(TagName const &tag) {
    static constexpr Array SPECIAL{
#define SPECIAL(NAME) NAME,
#include "defs/ns-html-special.inc"
#undef SPECIAL
    };

    return contains(SPECIAL, tag);
}

// 13.2.4.3 MARK: The list of active formatting elements
// https://html.spec.whatwg.org/multipage/parsing.html#list-of-active-formatting-elements

void reconstructActiveFormattingElements(HtmlParser &) {
    // TODO
}

// 13.2.5 MARK: Tokenization
// https://html.spec.whatwg.org/multipage/parsing.html#tokenization

// https://html.spec.whatwg.org/multipage/parsing.html#acknowledge-self-closing-flag
void acknowledgeSelfClosingFlag(HtmlToken const &) {
    logDebugIf(DEBUG_HTML_PARSER, "acknowledgeSelfClosingFlag not implemented");
}

// 13.2.6 MARK: Tree construction
// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

// 13.2.6.1 MARK: Creating and inserting nodes
// https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes

struct AdjustedInsertionLocation {
    Strong<Element> parent;

    // https://html.spec.whatwg.org/multipage/parsing.html#insert-an-element-at-the-adjusted-insertion-location
    void insert(Strong<Node> node) {
        // NOSPEC
        parent->appendChild(node);
    }

    Opt<Strong<Node>> lastChild() {
        // NOSPEC
        if (not parent->hasChildren())
            return NONE;
        return parent->lastChild();
    }
};

// https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
AdjustedInsertionLocation apropriatePlaceForInsertingANode(HtmlParser &b, Opt<Strong<Element>> overrideTarget = NONE) {
    // 1. If there was an override target specified, then let target be
    //    the override target.
    //
    //    Otherwise, let target be the current node.
    auto target = overrideTarget
                      ? *overrideTarget
                      : last(b._openElements);

    // 2. Determine the adjusted insertion location using the first
    //    matching steps from the following list:

    //    If foster parenting is enabled and target is a table, tbody, tfoot, thead, or tr element

    //    NOTE: Foster parenting happens when content is misnested in tables.

    //    1. Let last template be the last template element in the stack of open elements, if any.

    //    2. Let last table be the last table element in the stack of open elements, if any.

    //    3. If there is a last template and either there is no last table,
    //       or there is one, but last template is lower (more recently added)
    //       than last table in the stack of open elements,
    //       then: let adjusted insertion location be inside last template's
    //       template contents, after its last child (if any), and abort these steps.

    //    4. If there is no last table, then let adjusted insertion location be
    //       inside the first element in the stack of open elements (the html element),
    //       after its last child (if any), and abort these steps. (fragment case)

    //    5. If last table has a parent node, then let adjusted insertion location
    //       be inside last table's parent node, immediately before last table,
    //       and abort these steps.

    //    6. Let previous element be the element immediately above last table
    //       in the stack of open elements.

    //    7. Let adjusted insertion location be inside previous element,
    //       after its last child (if any).

    //  Otherwise: Let adjusted insertion location be inside target,
    //             after its last child (if any).

    // 3. If the adjusted insertion location is inside a template element,
    //    let it instead be inside the template element's template contents,
    //    after its last child (if any).

    // 4. Return the adjusted insertion location.
    return {target};
}

// https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
Strong<Element> createElementFor(HtmlToken const &t, Ns ns) {
    // NOSPEC: Keep it simple for the POC

    // 1. If the active speculative HTML parser is not null, then return the
    //    result of creating a speculative mock element given given namespace,
    //    the tag name of the given token, and the attributes of the given token.

    // 2. Otherwise, optionally create a speculative mock element given given
    //    namespace, the tag name of the given token, and the attributes of
    //    the given token.

    // 3. Let document be intended parent's node document.

    // 4. Let local name be the tag name of the token

    // 5. Let is be the value of the "is" attribute in the given token, if
    //    such an attribute exists, or null otherwise.

    // 6. Let definition be the result of looking up a custom element
    //    definition given document, given namespace, local name, and is.

    // 7. If definition is non-null and the parser was not created as part
    //    of the HTML fragment parsing algorithm, then let will execute
    //    script be true. Otherwise, let it be false.

    // NOSPEC: We don't support scripting so we don't need to worry about this
    bool willExecuteScript = false;

    // 8. If will execute script is true, then:

    if (willExecuteScript) {
        // 1. Increment document's throw-on-dynamic-markup-insertion counter.

        // 2. If the JavaScript execution context stack is empty,
        //    then perform a microtask checkpoint.

        // 3. Push a new element queue onto document's relevant agent's
        //    custom element reactions stack.
    }

    // 9. Let element be the result of creating an element given document,
    //    localName, given namespace, null, and is. If will execute script
    //    is true, set the synchronous custom elements flag; otherwise,
    //    leave it unset.
    auto tag = TagName::make(t.name, ns);
    logDebugIf(DEBUG_HTML_PARSER, "Creating element: {} {}", t.name, tag);
    auto el = makeStrong<Element>(tag);

    // 10. Append each attribute in the given token to element.
    for (auto &[name, value] : t.attrs) {
        el->setAttribute(AttrName::make(name, ns), value);
    }

    // 11. If will execute script is true, then:
    if (willExecuteScript) {
        // 1. Let queue be the result of popping from document's relevant
        //    agent's custom element reactions stack. (This will be the
        //    same element queue as was pushed above.)

        // 2. Invoke custom element reactions in queue.

        // 3. Decrement document's throw-on-dynamic-markup-insertion counter.
    }

    // 12. If element has an xmlns attribute in the XMLNS namespace whose
    //     value is not exactly the same as the element's namespace, that
    //     is a parse error. Similarly, if element has an xmlns:xlink
    //     attribute in the XMLNS namespace whose value is not the XLink
    //     Namespace, that is a parse error.

    // 13. If element is a resettable element, invoke its reset algorithm.
    //     (This initializes the element's value and checkedness based on the element's attributes.)

    // 14. If element is a form-associated element and not a form-associated
    //     custom element, the form element pointer is not null, there is no
    //     template element on the stack of open elements, element is either
    //     not listed or doesn't have a form attribute, and the intended parent
    //     is in the same tree as the element pointed to by the form element pointer,

    //     Then associate element with the form element pointed to by the form
    //     element pointer and set element's parser inserted flag.

    // 15. Return element.
    return el;
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-a-foreign-element

static Strong<Element> insertAForeignElement(HtmlParser &b, HtmlToken const &t, Ns ns, bool onlyAddToElementStack = false) {
    // 1. Let the adjusted insertion location be the appropriate place for inserting a node.
    auto location = apropriatePlaceForInsertingANode(b);

    // 2. Let element be the result of creating an element for the token in the
    // given namespace, with the intended parent being the element in which the
    // adjusted insertion location finds itself.
    auto el = createElementFor(t, ns);

    // 3. If onlyAddToElementStack is false, then run insert an element at the adjusted insertion location with element.
    if (not onlyAddToElementStack) {
        location.insert(el);
    }

    // 4. Push element onto the stack of open elements so that it is the new current node.
    b._openElements.pushBack(el);

    // 5. Return element.
    return el;
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-an-html-element
static Strong<Element> insertHtmlElement(HtmlParser &b, HtmlToken const &t) {
    return insertAForeignElement(b, t, Vaev::HTML, false);
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-a-character
static void insertACharacter(HtmlParser &b, Rune c) {
    // 2. Let the adjusted insertion location be the appropriate place for inserting a node.
    auto location = apropriatePlaceForInsertingANode(b);

    // 3. If the adjusted insertion location is inside a Document node, then ignore the token.
    if (location.parent->nodeType() == NodeType::DOCUMENT)
        return;

    // 4. If there is a Text node immediately before the adjusted insertion
    //    location, then append data to that Text node's data.
    auto lastChild = location.lastChild();
    if (lastChild and (*lastChild)->nodeType() == NodeType::TEXT) {
        auto text = (*(*lastChild).cast<Text>());
        text->appendData(c);
    }

    // Otherwise, create a new Text node whose data is data and whose node
    //            document is the same as that of the element in which the
    //            adjusted insertion location finds itself, and insert the
    //            newly created node at the adjusted insertion location.
    else {
        auto text = makeStrong<Text>(""s);
        text->appendData(c);

        location.insert(text);
    }
}

static void insertACharacter(HtmlParser &b, HtmlToken const &t) {
    // 1. Let data be the characters passed to the algorithm, or, if no characters were explicitly specified, the character of the character token being processed.
    insertACharacter(b, t.rune);
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-a-comment
static void insertAComment(HtmlParser &b, HtmlToken const &t) {
    // 1. Let data be the data given in the comment token being processed.

    // 2. If position was specified, then let the adjusted insertion
    //    location be position. Otherwise, let adjusted insertion location
    //    be the appropriate place for inserting a node.

    // TODO: If position was
    auto location = apropriatePlaceForInsertingANode(b);

    // 3. Create a Comment node whose data attribute is set to data and
    //    whose node document is the same as that of the node in which
    //    the adjusted insertion location finds itself.
    auto comment = makeStrong<Comment>(t.data);

    // 4. Insert the newly created node at the adjusted insertion location.
    location.insert(comment);
}

// 13.2.6.2 MARK: Parsing elements that contain only text
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-elements-that-contain-only-text

static void parseRawTextElement(HtmlParser &b, HtmlToken const &t) {
    insertHtmlElement(b, t);
    b._lexer._switchTo(HtmlLexer::RAWTEXT);
    b._originalInsertionMode = b._insertionMode;
    b._switchTo(HtmlParser::Mode::TEXT);
}

static void parseRcDataElement(HtmlParser &b, HtmlToken const &t) {
    insertHtmlElement(b, t);
    b._lexer._switchTo(HtmlLexer::RCDATA);
    b._originalInsertionMode = b._insertionMode;
    b._switchTo(HtmlParser::Mode::TEXT);
}

// 13.2.6.3 MARK: Closing elements that have implied end tags
// https://html.spec.whatwg.org/multipage/parsing.html#generate-implied-end-tags
static constexpr Array IMPLIED_END_TAGS = {
    Html::DD, Html::DT, Html::LI, Html::OPTION, Html::OPTGROUP, Html::P, Html::RB, Html::RP, Html::RT, Html::RTC
};

static void generateImpliedEndTags(HtmlParser &b, Str except = ""s) {
    while (contains(IMPLIED_END_TAGS, last(b._openElements)->tagName) and
           last(b._openElements)->tagName.name() != except) {
        b._openElements.popBack();
    }
}

// 13.2.6.4 MARK: The rules for parsing tokens in HTML content

// 13.2.6.4.1 MARK: The "initial" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode

static QuirkMode _whichQuirkMode(HtmlToken const &) {
    // NOSPEC: We assume no quirk mode
    return QuirkMode::NO;
}

void HtmlParser::_handleInitialMode(HtmlToken const &t) {
    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // ignore
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        _document->appendChild(makeStrong<Comment>(t.data));
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        _document->appendChild(makeStrong<DocumentType>(
            t.name,
            t.publicIdent,
            t.systemIdent
        ));
        _document->quirkMode = _whichQuirkMode(t);
        _switchTo(Mode::BEFORE_HTML);
    }

    // Anything else
    else {
        _switchTo(Mode::BEFORE_HTML);
        accept(t);
    }
}

// 13.2.6.4.2 MARK: The "before html" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
void HtmlParser::_handleBeforeHtml(HtmlToken const &t) {
    // A DOCTYPE token
    if (t.type == HtmlToken::DOCTYPE) {
        // ignore
        _raise();
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        _document->appendChild(makeStrong<Comment>(t.data));
    }

    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // ignore
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and t.name == "html") {
        auto el = createElementFor(t, Vaev::HTML);
        _document->appendChild(el);
        _openElements.pushBack(el);
        _switchTo(Mode::BEFORE_HEAD);
    }

    // Any other end tag
    else if (t.type == HtmlToken::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    }

    // An end tag whose tag name is one of: "head", "body", "html", "br"
    // Anything else
    else {
        auto el = makeStrong<Element>(Html::HTML);
        _document->appendChild(el);
        _openElements.pushBack(el);
        _switchTo(Mode::BEFORE_HEAD);
        accept(t);
    }
}

// 13.2.6.4.3 MARK: The "before head" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
void HtmlParser::_handleBeforeHead(HtmlToken const &t) {
    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // Ignore the token.
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        // Insert a comment.
        insertAComment(*this, t);
    }

    // A comment token
    else if (t.type == HtmlToken::DOCTYPE) {
        // Parse error. Ignore the token.
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and t.name == "html") {
        // Process the token using the rules for the "in body" insertion mode.
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is "head"
    else if (t.type == HtmlToken::START_TAG and t.name == "head") {
        _headElement = insertHtmlElement(*this, t);
        _switchTo(Mode::IN_HEAD);
    }

    // Anything else
    else if (t.type == HtmlToken::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    }

    // An end tag whose tag name is one of: "head", "body", "html", "br"
    // Anything else
    else {
        HtmlToken headToken;
        headToken.type = HtmlToken::START_TAG;
        headToken.name = String{"head"};
        _headElement = insertHtmlElement(*this, headToken);
        _switchTo(Mode::IN_HEAD);
        accept(t);
    }
}

// 13.2.6.4.4 MARK: The "in head" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
void HtmlParser::_handleInHead(HtmlToken const &t) {
    auto anythingElse = [&] {
        _openElements.popBack();
        _switchTo(Mode::AFTER_HEAD);
        accept(t);
    };

    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        insertACharacter(*this, t);
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        insertAComment(*this, t);
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is one of: "base", "basefont", "bgsound", "link"
    else if (t.type == HtmlToken::START_TAG and (t.name == "base" or t.name == "basefont" or t.name == "bgsound" or t.name == "link")) {
        insertHtmlElement(*this, t);
        _openElements.popBack();
        // TODO: Acknowledge the token's self-closing flag, if it is set.
    }

    // A start tag whose tag name is "meta"
    else if (t.type == HtmlToken::START_TAG and (t.name == "meta")) {
        insertHtmlElement(*this, t);
        _openElements.popBack();
        // TODO: Acknowledge the token's self-closing flag, if it is set.

        // TODO: Handle handle speculative parsing
    }

    // A start tag whose tag name is "title"
    else if (t.type == HtmlToken::START_TAG and (t.name == "title")) {
        parseRcDataElement(*this, t);
    }

    // A start tag whose tag name is "noscript", if the scripting flag is enabled
    // A start tag whose tag name is one of: "noframes", "style"
    else if (t.type == HtmlToken::START_TAG and ((t.name == "noscript" and _scriptingEnabled) or t.name == "noframe" or t.name == "style")) {
        parseRawTextElement(*this, t);
    }

    // A start tag whose tag name is "noscript", if the scripting flag is disabled
    else if (t.type == HtmlToken::START_TAG and (t.name == "noscript" and not _scriptingEnabled)) {
        insertHtmlElement(*this, t);
        _switchTo(Mode::IN_HEAD_NOSCRIPT);
    }

    // A start tag whose tag name is "script"
    else if (t.type == HtmlToken::START_TAG and (t.name == "script")) {
        // 1. Let the adjusted insertion location be the appropriate place for inserting a node.
        auto localtion = apropriatePlaceForInsertingANode(*this);

        // 2. Create an element for the token in the HTML namespace, with
        //    the intended parent being the element in which the adjusted
        //    insertion location finds itself.
        auto el = createElementFor(t, Vaev::HTML);

        // 3. Set the element's parser document to the Document, and set
        //    the element's force async to false.

        // NOSPEC: We don't support async scripts

        // NOTE: This ensures that, if the script is external,
        //       any document.write() calls in the script will execute
        //       in-line, instead of blowing the document away, as would
        //       happen in most other cases. It also prevents the script
        //       from executing until the end tag is seen.

        // 4. If the parser was created as part of the HTML fragment
        //    parsing algorithm, then set the script element's already
        //    started to true. (fragment case)

        // NOSPEC: We don't support fragments

        // 5. If the parser was invoked via the document.write() or
        //    document.writeln() methods, then optionally set the script
        //    element's already started to true. (For example, the user
        //    agent might use this clause to prevent execution of
        //    cross-origin scripts inserted via document.write() under
        //    slow network conditions, or when the page has already taken
        //    a long time to load.)

        // NOSPEC: We don't support document.write()

        // 6. Insert the newly created element at the adjusted insertion location.
        localtion.insert(el);

        // 7. Push the element onto the stack of open elements so that it is the new current node.
        _openElements.pushBack(el);

        // 8. Switch the tokenizer to the script data state.
        _lexer._switchTo(HtmlLexer::SCRIPT_DATA);

        // 9. Let the original insertion mode be the current insertion mode.
        _originalInsertionMode = _insertionMode;

        // 10. Switch the insertion mode to "text".
        _switchTo(Mode::TEXT);
    } else if (t.type == HtmlToken::END_TAG and (t.name == "head")) {
        _openElements.popBack();
        _switchTo(Mode::AFTER_HEAD);
    } else if (t.type == HtmlToken::END_TAG and (t.name == "body" or t.name == "html" or t.name == "br")) {
        anythingElse();
    } else if (t.type == HtmlToken::START_TAG and (t.name == "template")) {
        // NOSPEC: We don't support templates
    } else if (t.type == HtmlToken::END_TAG and (t.name == "template")) {
        // NOSPEC: We don't support templates
    } else if ((t.type == HtmlToken::START_TAG and (t.name == "head")) or t.type == HtmlToken::END_TAG) {
        // ignore
        _raise();
    } else {
        anythingElse();
    }
}

// 13.2.6.4.5 MARK: The "in head noscript" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inheadnoscript
void HtmlParser::_handleInHeadNoScript(HtmlToken const &t) {
    auto anythingElse = [&] {
        _raise();
        _openElements.popBack();
        _switchTo(Mode::IN_HEAD);
        accept(t);
    };

    // A DOCTYPE token
    if (t.type == HtmlToken::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // An end tag whose tag name is "noscript"
    else if (t.type == HtmlToken::END_TAG and (t.name == "noscript")) {
        _openElements.popBack();
        _switchTo(Mode::IN_HEAD);
    }

    // A character token that is one of
    //   - U+0009 CHARACTER TABULATION,
    //   - U+000A LINE FEED (LF),
    //   - U+000C FORM FEED (FF),
    //   - U+000D CARRIAGE RETURN (CR),
    //   - U+0020 SPACE
    // A comment token
    // A start tag whose tag name is one of: "basefont", "bgsound", "link", "meta", "noframes", "style"
    else if (
        (t.type == HtmlToken::CHARACTER and
         (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == ' ')) or
        t.type == HtmlToken::COMMENT or
        (t.type == HtmlToken::START_TAG and
         (t.name == "basefont" or t.name == "bgsound" or t.name == "link" or t.name == "meta" or t.name == "noframes" or t.name == "style"))
    ) {
        _acceptIn(Mode::IN_HEAD, t);
    }

    // An end tag whose tag name is "br"
    else if (t.type == HtmlToken::END_TAG and (t.name == "br")) {
        anythingElse();
    }

    // A start tag whose tag name is one of: "head", "noscript"
    // Any other end tag
    else if (
        (t.type == HtmlToken::START_TAG and (t.name == "head" or t.name == "noscript")) or
        t.type == HtmlToken::END_TAG
    ) {
        // ignore
        _raise();
    }

    // Anything else
    else {
        anythingElse();
    }
}

// 13.2.6.4.6 MARK: The "after head" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-after-head-insertion-mode
void HtmlParser::_handleAfterHead(HtmlToken const &t) {
    auto anythingElse = [&] {
        HtmlToken bodyToken;
        bodyToken.type = HtmlToken::START_TAG;
        bodyToken.name = String{"body"};
        insertHtmlElement(*this, bodyToken);
        _switchTo(Mode::IN_BODY);
        accept(t);
    };

    // A character token that is one of
    //   - U+0009 CHARACTER TABULATION,
    //   - U+000A LINE FEED (LF),
    //   - U+000C FORM FEED (FF),
    //   - U+000D CARRIAGE RETURN (CR)
    //   - U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')) {
        insertACharacter(*this, t.rune);
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        insertAComment(*this, t);
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is "body"
    else if (t.type == HtmlToken::START_TAG and (t.name == "body")) {
        insertHtmlElement(*this, t);
        _framesetOk = false;
        _switchTo(Mode::IN_BODY);
    }

    // A start tag whose tag name is "frameset"
    else if (t.type == HtmlToken::START_TAG and (t.name == "frameset")) {
        insertHtmlElement(*this, t);
        _switchTo(Mode::IN_FRAMESET);
    }

    // A start tag whose tag name is one of:
    //   "base", "basefont", "bgsound", "link", "meta",
    //   "noframes", "script", "style", "template", "title"
    else if (
        t.type == HtmlToken::START_TAG and
        (t.name == "base" or t.name == "basefont" or
         t.name == "bgsound" or t.name == "link" or
         t.name == "meta" or t.name == "noframes" or
         t.name == "script" or t.name == "style" or
         t.name == "template" or t.name == "title")
    ) {
        _raise();
        _openElements.pushBack(*_headElement);
        _acceptIn(Mode::IN_HEAD, t);
        _openElements.removeAll(*_headElement);
    }

    // An end tag whose tag name is "template"
    else if (t.type == HtmlToken::END_TAG and (t.name == "template")) {
        _acceptIn(Mode::IN_HEAD, t);
    }

    // An end tag whose tag name is one of: "body", "html", "br"
    else if (t.type == HtmlToken::END_TAG and (t.name == "body" or t.name == "html" or t.name == "br")) {
        anythingElse();
    }

    // A start tag whose tag name is "head"
    else if (t.type == HtmlToken::END_TAG or (t.type == HtmlToken::START_TAG and t.name == "head")) {
        // ignore
        _raise();
    }

    // Anything else
    else {
        anythingElse();
    }
}

// 13.2.6.4.7 MARK: The "in body" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
void HtmlParser::_handleInBody(HtmlToken const &t) {
    // A character token that is U+0000 NULL
    if (t.type == HtmlToken::CHARACTER and t.rune == '\0') {
        _raise();
    }

    // A character token that is one of
    //   - U+0009 CHARACTER TABULATION
    //   - U+000A LINE FEED (LF)
    //   - U+000C FORM FEED (FF)
    //   - U+000D CARRIAGE RETURN (CR)
    //   - U+0020 SPACE
    else if (t.type == HtmlToken::CHARACTER and (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')) {
        reconstructActiveFormattingElements(*this);
        insertACharacter(*this, t);
    }

    // Any other character token
    else if (t.type == HtmlToken::CHARACTER) {
        reconstructActiveFormattingElements(*this);
        insertACharacter(*this, t);
        _framesetOk = false;
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        insertAComment(*this, t);
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        _raise();
    }

    // TODO: A start tag whose tag name is "html"

    // TODO: A start tag whose tag name is one of: "base", "basefont", "bgsound", "link", "meta", "noframes", "script", "style", "template", "title"
    // An end tag whose tag name is "template"

    // TODO: A start tag whose tag name is "body"

    // TODO: A start tag whose tag name is "frameset"

    // TODO: An end-of-file token

    // TODO: An end tag whose tag name is "body"
    else if (t.type == HtmlToken::END_TAG and t.name == "body") {
        // If the stack of open elements does not have a body element in
        // scope, this is a parse error; ignore the token.
        if (not _hasElementInScope(Html::BODY)) {
            _raise();
            return;
        }

        // Otherwise, if there is a node in the stack of open elements that
        // is not a implid end tag, then this is a parse error.
        if (not contains(IMPLIED_END_TAGS, last(_openElements)->tagName)) {
            _raise();
        }

        // Switch the insertion mode to "after body".
        _switchTo(Mode::AFTER_BODY);
    }

    // TODO: An end tag whose tag name is "html"

    // TODO: A start tag whose tag name is one of:
    // "address", "article", "aside", "blockquote", "center",
    // "details", "dialog", "dir", "div", "dl", "fieldset",
    // "figcaption", "figure", "footer", "header", "hgroup",
    // "main", "menu", "nav", "ol", "p", "search", "section",
    // "summary", "ul"

    // TODO: A start tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"

    // TODO: A start tag whose tag name is one of: "pre", "listing"

    // TODO: A start tag whose tag name is "form"

    // TODO: A start tag whose tag name is "li"

    // TODO: A start tag whose tag name is one of: "dd", "dt"

    // TODO: A start tag whose tag name is "plaintext"

    // TODO: A start tag whose tag name is "button"

    // TODO: An end tag whose tag name is one of: "address", "article", "aside", "blockquote", "button", "center", "details", "dialog", "dir", "div", "dl", "fieldset", "figcaption", "figure", "footer", "header", "hgroup", "listing", "main", "menu", "nav", "ol", "pre", "search", "section", "summary", "ul"

    // TODO: An end tag whose tag name is "form"

    // TODO: An end tag whose tag name is "p"

    // TODO: An end tag whose tag name is "li"

    // TODO: An end tag whose tag name is one of: "dd", "dt"

    // TODO: An end tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"

    // TODO: An end tag whose tag name is "sarcasm"
    //       This state machine is not equipped to handle sarcasm
    //       So we just ignore it

    // TODO: A start tag whose tag name is "a"

    // TODO: A start tag whose tag name is one of: "b", "big", "code", "em", "font", "i", "s", "small", "strike", "strong", "tt", "u"

    // TODO: A start tag whose tag name is "nobr"

    // TODO: An end tag whose tag name is one of: "a", "b", "big", "code", "em", "font", "i", "nobr", "s", "small", "strike", "strong", "tt", "u"

    // TODO: A start tag whose tag name is one of: "applet", "marquee", "object"

    // TODO: An end tag token whose tag name is one of: "applet", "marquee", "object"

    // TODO: A start tag whose tag name is "table"

    // TODO: An end tag whose tag name is "br"

    // TODO: A start tag whose tag name is one of: "area", "br", "embed", "img", "keygen", "wbr"

    // TODO: A start tag whose tag name is "input"

    // TODO: A start tag whose tag name is one of: "param", "source", "track"

    // TODO: A start tag whose tag name is "hr"

    // TODO: A start tag whose tag name is "image"

    // TODO: A start tag whose tag name is "textarea"

    // TODO: A start tag whose tag name is "xmp"

    // TODO: A start tag whose tag name is "iframe"

    // TODO: A start tag whose tag name is "noembed"
    // A start tag whose tag name is "noscript", if the scripting flag is enabled

    // TODO: A start tag whose tag name is "select"

    // TODO: A start tag whose tag name is one of: "optgroup", "option"

    // TODO: A start tag whose tag name is one of: "rb", "rtc"

    // TODO: A start tag whose tag name is one of: "rp", "rt"

    // TODO: A start tag whose tag name is "math"

    // TODO: A start tag whose tag name is "svg"

    // TODO: A start tag whose tag name is one of: "caption", "col", "colgroup", "frame", "head", "tbody", "td", "tfoot", "th", "thead", "tr"

    else if (t.type == HtmlToken::START_TAG) {
        reconstructActiveFormattingElements(*this);
        insertHtmlElement(*this, t);
    }

    // TODO: Any other end tag
    else if (t.type == HtmlToken::END_TAG) {
        // loop:
        usize curr = _openElements.len();
        while (curr > 0) {
            // 1. Initialize node to be the current node (the bottommost node of the stack).
            auto node = _openElements[curr - 1];

            // 2. Loop: If node is an HTML element with the same tag name as the token, then:
            if (node->tagName.name() == t.name) {
                // 1. Generate implied end tags, except for HTML elements with the same tag name as the token.
                generateImpliedEndTags(*this, t.name);

                // 2. If node is not the current node, then this is a parse error.
                if (node != last(_openElements))
                    _raise();

                // 3. Pop all the nodes from the current node up to node, including node, then stop these steps
                while (last(_openElements) != node) {
                    _openElements.popBack();
                }
                _openElements.popBack();
                break;
            }

            // 3. Otherwise, if node is in the special category,
            //    then this is a parse error; ignore the token, and return.

            // TODO: Implement the special category

            // 4. Set node to the previous entry in the stack of open elements.
            curr--;

            // 5. Return to the step labeled loop.
        }
    }
}

// 13.2.6.4.8 MARK: The "text" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incdata
void HtmlParser::_handleText(HtmlToken const &t) {
    // A character token
    if (t.type == HtmlToken::CHARACTER) {
        insertACharacter(
            *this,
            t.rune == '\0'
                ? 0xFFFD
                : t.rune
        );
    }

    else if (t.type == HtmlToken::END_OF_FILE) {
        _raise();

        // TODO: If the current node is a script element, then set its already started to true.

        _openElements.popBack();
        _switchTo(_originalInsertionMode);
    }

    // An end tag whose tag name is "script"
    // else if (t.type == Token::END_TAG and t.name == "script") {
    // }
    // NOSPEC: We handle script end tags like any other end tag

    // Any other end tag
    else if (t.type == HtmlToken::END_TAG) {
        this->_openElements.popBack();
        _switchTo(_originalInsertionMode);
    }

    // FIXME: Implement the rest of the rules
}

// 3.2.6.4.22 MARK: The "after after body" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-body-insertion-mode
void HtmlParser::_handleAfterBody(HtmlToken const &t) {
    // A comment token
    if (t.type == HtmlToken::COMMENT) {
        // Insert a comment.
        insertAComment(*this, t);
    }

    // A DOCTYPE token
    // A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::DOCTYPE or
             (t.type == HtmlToken::CHARACTER and (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')) or
             (t.type == HtmlToken::START_TAG and t.name == "html")) {
        // Process the token using the rules for the "in body" insertion mode.
        _acceptIn(Mode::IN_BODY, t);
    }

    else if (t.type == HtmlToken::END_OF_FILE) {
        // Stop parsing.
    }

    else {
        // Parse error. Switch the insertion mode to "in body" and reprocess the token.
        _raise();
        _switchTo(Mode::IN_BODY);
        accept(t);
    }
}

void HtmlParser::_switchTo(Mode mode) {
    _insertionMode = mode;
}

void HtmlParser::_acceptIn(Mode mode, HtmlToken const &t) {
    logDebugIf(DEBUG_HTML_PARSER, "Parsing {} in {}", t, mode);

    switch (mode) {

    case Mode::INITIAL:
        _handleInitialMode(t);
        break;

    case Mode::BEFORE_HTML:
        _handleBeforeHtml(t);
        break;

    case Mode::BEFORE_HEAD:
        _handleBeforeHead(t);
        break;

    case Mode::IN_HEAD:
        _handleInHead(t);
        break;

    case Mode::IN_HEAD_NOSCRIPT:
        _handleInHeadNoScript(t);
        break;

    case Mode::AFTER_HEAD:
        _handleAfterHead(t);
        break;

    case Mode::IN_BODY:
        _handleInBody(t);
        break;

    case Mode::TEXT:
        _handleText(t);
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
    case Mode::IN_TABLE:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intabletext
    case Mode::IN_TABLE_TEXT:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incaption
    case Mode::IN_CAPTION:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incolumngroup
    case Mode::IN_COLUMN_GROUP:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intablebody
    case Mode::IN_TABLE_BODY:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inrow
    case Mode::IN_ROW:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incell
    case Mode::IN_CELL:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselect
    case Mode::IN_SELECT:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselectintable
    case Mode::IN_SELECT_IN_TABLE:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intemplate
    case Mode::IN_TEMPLATE:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-body-insertion-mode
    case Mode::AFTER_BODY:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-in-frameset-insertion-mode
    case Mode::IN_FRAMESET:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-frameset-insertion-mode
    case Mode::AFTER_FRAMESET:
        break;

    case Mode::AFTER_AFTER_BODY:
        _handleAfterBody(t);
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-frameset-insertion-mode
    case Mode::AFTER_AFTER_FRAMESET:
        break;

    default:
        break;
    }
}

void HtmlParser::accept(HtmlToken const &t) {
    _acceptIn(_insertionMode, t);
}

} // namespace Vaev::Markup
