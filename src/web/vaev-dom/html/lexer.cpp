#include <karm-logger/logger.h>

#include "lexer.h"

namespace Vaev::Dom {

static constexpr bool DEBUG_HTML_LEXER = false;

struct Entity {
    Str name;
    Rune const* runes;

    operator bool() {
        return runes != nullptr;
    }
};

[[maybe_unused]] static Array const ENTITIES = {
#define ENTITY(NAME, ...) \
    Entity{#NAME, (Rune[]){__VA_ARGS__ __VA_OPT__(, ) 0}},
#include "../defs/ns-html-entities.inc"
#undef ENTITY
};

void HtmlLexer::_raise(Str msg) {
    logError("{}: {}", _state, msg);
}

void HtmlLexer::consume(Rune rune, bool isEof) {
    logDebugIf(DEBUG_HTML_LEXER, "Lexing '{#c}' {#x} in {}", rune, rune, _state);

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
            _builder.clear();
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
            _builder.clear();
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
            _ensure(HtmlToken::COMMENT).data = _commentBuilder.take();
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
            _commentBuilder.append(0xFFFD);
        }

        // Anything else
        // Append the current input character to the comment token's data.
        else {
            _commentBuilder.append(rune);
        }

        break;
    }

    case State::MARKUP_DECLARATION_OPEN: {
        // 13.2.5.42 MARK: Markup declaration open state
        // If the next few characters are:

        peekerForSingleState.append(rune);

        // Two U+002D HYPHEN-MINUS characters (-)
        // Consume those two characters, create a comment token whose data
        // is the empty string, and switch to the comment start state.
        if (auto r = startWith("--"s, peekerForSingleState.str()); r != Match::NO) {
            if (r == Match::PARTIAL)
                break;

            peekerForSingleState.clear();
            _begin(HtmlToken::COMMENT);
            _switchTo(State::COMMENT_START);
        }

        // ASCII case-insensitive match for the word "DOCTYPE"
        // Consume those characters and switch to the DOCTYPE state.

        else if (auto r = startWith("DOCTYPE"s, peekerForSingleState.str()); r != Match::NO) {
            if (r == Match::PARTIAL)
                break;

            peekerForSingleState.clear();
            _switchTo(State::DOCTYPE);
        }

        // The string "[CDATA[" (the five uppercase letters "CDATA" with a
        // U+005B LEFT SQUARE BRACKET character before and after) Consume
        // those characters. If there is an adjusted current node and it is
        // not an element in the HTML namespace, then switch to the CDATA
        // section state. Otherwise, this is a cdata-in-html-content parse
        // error. Create a comment token whose data is the "[CDATA[" string.
        // Switch to the bogus comment state.

        else if (auto r = startWith("[CDATA["s, peekerForSingleState.str()); r != Match::NO) {
            if (r == Match::PARTIAL)
                break;

            // NOSPEC: This is in reallity more complicated
            peekerForSingleState.clear();
            _switchTo(State::CDATA_SECTION);
        }

        // Anything else
        // This is an incorrectly-opened-comment parse error. Create a
        // comment token whose data is the empty string. Switch to the bogus
        // comment state (don't consume anything in the current state).
        else {
            peekerForSingleState.clear();
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
            _ensure(HtmlToken::COMMENT).data = _commentBuilder.take();
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
            _commentBuilder.append('-');
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
            _commentBuilder.append(rune);
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
            _commentBuilder.append(0xFFFD);
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
            _commentBuilder.append(rune);
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
            _commentBuilder.append(rune);
            _switchTo(State::COMMENT_LESS_THAN_SIGN_BANG);
        }

        // U+003C LESS-THAN SIGN (<)
        // Append the current input character to the comment token's data.
        else if (rune == '<') {
            _commentBuilder.append(rune);
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
            _commentBuilder.append('-');
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
            _ensure(HtmlToken::COMMENT).data = _commentBuilder.take();
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
            _commentBuilder.append('-');
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
            _commentBuilder.append('-');
            _commentBuilder.append('-');
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
            _commentBuilder.append('-');
            _commentBuilder.append('-');
            _commentBuilder.append('!');
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
            _commentBuilder.append('-');
            _commentBuilder.append('-');
            _commentBuilder.append('!');
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
        // 13.2.5.73 MARK: Named character reference state

        // Consume the maximum number of characters possible, where the
        // consumed characters are one of the identifiers in the first
        // column of the named character references table. Append each
        // character to the temporary buffer when it's consumed.

        // NOTE: This state asks us to "Consume the maximum number of characters possible" but also to check the
        // "next input character"; an extra implementation effort was made in order to be able to access
        // the "next input character"

        // NOTE: While the default and valid behaviour is for entities to end with semicolon (;), some entities have
        // 2 names, one ending with semicolon and the other not.
        // For these cases, even if we have a match, if it doesnt end with semi-colon, we will continue consuming

        // NOTE: Some entities are prefix of others: "&not" is prefix of "&notinva;"; however, since ";" only appears if
        // its the last char, it cant be that a string ending with it is also a prefix

        // NOTE: In this state, we only add alphanum chars to _temp

        // NOTE: In case of matching just a prefix or not matching at all but having already consumed characters into
        // _temp, it is ok to rely on _flushCodePointsConsumedAsACharacterReference for correctly dispatch the runes in
        // _temp.
        // In the case where there is no match at all, the next state is State::AMBIGUOUS_AMPERSAND which emits the
        // consumed alphanum.
        //      Eg. given the entity "&between;", the input "&betweem" will be emited as State::AMBIGUOUS_AMPERSAND
        //      does
        // For return states, attribute states add alphanum chars to their builder and data states emit alphanum
        //      Eg. given the entities "&not" and "&notinva;", the input "&notinvd" matches "&not" and
        //      flushes "invd" as the return state would

        bool hasPartialMatch = false;

        auto computeMatchState = [&](StringBuilder prefix) {
            // TODO: (performance) consider sorting entities lexicographically and binary search or using a trie

            prefix.append(rune);

            auto target = prefix.str();

            auto bestMatch = Match::NO;
            for (auto& entity : ENTITIES) {
                auto match = startWith(entity.name, target);

                if (match == Match::PARTIAL)
                    hasPartialMatch = true;

                if (match == Match::YES)
                    bestMatch = max(bestMatch, match);
            };
            return bestMatch;
        };

        auto matchStateWithNextInputChar = computeMatchState(_temp);

        // NOTE: if rune==';', we are either having Match::YES or Match::NO, not partial
        if (hasPartialMatch) {
            _temp.append(rune);

            if (matchStateWithNextInputChar == Match::YES)
                matchedCharReferenceNoSemiColon = _temp.len();

            break;
        }

        // If there is a match from before matchedCharReferenceNoSemiColon
        if (matchStateWithNextInputChar == Match::NO and matchedCharReferenceNoSemiColon) {
            // If the character reference was consumed as part of an attribute,
            // and the last character matched is not a U+003B SEMICOLON
            // character (;), and the next input character is either a U+003D
            // EQUALS SIGN character (=) or an ASCII alphanumeric, then, for
            // historical reasons, flush code points consumed as a character
            // reference and switch to the return state.
            if (
                _consumedAsPartOfAnAttribute() and
                (rune == '=' or isAsciiAlphaNum(rune))
            ) {
                _flushCodePointsConsumedAsACharacterReference();
                _reconsumeIn(_returnState, rune);
            } else {
                // Otherwise:

                // If the last character matched is not a U+003B SEMICOLON character
                // (;), then this is a missing-semicolon-after-character-reference
                // parse error.
                _raise("missing-semicolon-after-character-reference");

                // Set the temporary buffer to the empty string.
                // Append one or two characters corresponding to the character reference name (as
                // given by the second column of the named character references
                // table) to the temporary buffer.

                // NOTE: we should expand the entity but also re-add the not matched remaining characters
                // to the _temp buffer

                auto _tempWithUnexpandedEntity = _temp.str();
                auto entityName = _Str<Utf8>(_tempWithUnexpandedEntity.begin(), matchedCharReferenceNoSemiColon.unwrap());

                for (auto& entity : ENTITIES) {
                    if (entityName == entity.name) {

                        _temp.clear();
                        _temp.append(Slice<Rune>::fromNullterminated(entity.runes));

                        for (usize i = matchedCharReferenceNoSemiColon.unwrap(); i < _tempWithUnexpandedEntity.len(); ++i) {
                            _temp.append(_tempWithUnexpandedEntity[i]);
                        }
                        break;
                    }
                }

                // Flush code points consumed as a character reference. Switch to
                // the return state.
                matchedCharReferenceNoSemiColon = NONE;
                _flushCodePointsConsumedAsACharacterReference();
                _reconsumeIn(_returnState, rune);
            }
        }

        else if (matchStateWithNextInputChar == Match::YES) {
            // FIXME: consider run this assert only in DEBUG mode
            if (rune != ';')
                panic("A full entity match that is not a partial match must end with a semicolon");

            _temp.append(rune);

            // If the character reference was consumed as part of an attribute,
            // and the last character matched is not a U+003B SEMICOLON
            // character (;), and the next input character is either a U+003D
            // EQUALS SIGN character (=) or an ASCII alphanumeric, then, for
            // historical reasons, flush code points consumed as a character
            // reference and switch to the return state.

            // => This is impossible since the last matched char is a semicolon

            // Otherwise:

            // If the last character matched is not a U+003B SEMICOLON character
            // (;), then this is a missing-semicolon-after-character-reference
            // parse error.

            // => This is impossible since the last matched char is a semicolon

            // Set the temporary buffer to the empty string.
            // Append one or two characters corresponding to the character reference name (as
            // given by the second column of the named character references
            // table) to the temporary buffer.
            for (auto& entity : ENTITIES) {
                if (_temp.str() == entity.name) {
                    _temp.clear();
                    _temp.append(Slice<Rune>::fromNullterminated(entity.runes));
                    break;
                }
            }

            // Flush code points consumed as a character reference. Switch to
            // the return state.
            matchedCharReferenceNoSemiColon = NONE;
            _flushCodePointsConsumedAsACharacterReference();
            _switchTo(_returnState);
        }

        else {
            // Otherwise Flush code points consumed as a character reference.
            // Switch to the ambiguous ampersand state.
            _flushCodePointsConsumedAsACharacterReference();
            _reconsumeIn(State::AMBIGUOUS_AMPERSAND, rune);
        }
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
            if (_consumedAsPartOfAnAttribute()) {
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
            Pair{0x80u, 0x20ACuz},
            Pair{0x82u, 0x201Auz},
            Pair{0x83u, 0x0192uz},
            Pair{0x84u, 0x201Euz},
            Pair{0x85u, 0x2026uz},
            Pair{0x86u, 0x2020uz},
            Pair{0x87u, 0x2021uz},
            Pair{0x88u, 0x02C6uz},
            Pair{0x89u, 0x2030uz},
            Pair{0x8Au, 0x0160uz},
            Pair{0x8Bu, 0x2039uz},
            Pair{0x8Cu, 0x0152uz},
            Pair{0x8Eu, 0x017Duz},
            Pair{0x91u, 0x2018uz},
            Pair{0x92u, 0x2019uz},
            Pair{0x93u, 0x201Cuz},
            Pair{0x94u, 0x201Duz},
            Pair{0x95u, 0x2022uz},
            Pair{0x96u, 0x2013uz},
            Pair{0x97u, 0x2014uz},
            Pair{0x98u, 0x02DCuz},
            Pair{0x99u, 0x2122uz},
            Pair{0x9Au, 0x0161uz},
            Pair{0x9Bu, 0x203Auz},
            Pair{0x9Cu, 0x0153uz},
            Pair{0x9Eu, 0x017Euz},
            Pair{0x9Fu, 0x0178uz},
        };

        for (auto& conv : CONV)
            if (conv.v0 == _currChar) {
                _currChar = conv.v1;
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

} // namespace Vaev::Dom
